#include <precompiled.h>
#include "Graphics_.h"
#include "EffectsManager_.h"
#include "Python_.h"
#include <tanita2/application/application.h>

// Save screenshot flag
bool Direct3DInstance::need_save_screenshot = false;

// Constructor
Direct3DInstance::Direct3DInstance( const Direct3DInstance::initializer & init )
    : is_device_lost(false), rtt_enabled(false),
      hardware_yuv_enabled(false), shaders_enabled(false),
      d3d(init.d3d), device(init.device)
{
    // Preparing device parameters
    Config conf;
    D3DPRESENT_PARAMETERS pp = init.params;
    pp.Windowed = conf->get<bool>("windowed");
    if (!pp.Windowed)
        pp.FullScreen_RefreshRateInHz = conf->get<int>("refresh_rate");

    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.BackBufferFormat = pp.Windowed ? D3DFMT_UNKNOWN : D3DFMT_X8R8G8B8;
    pp.BackBufferWidth = conf->get<int>("width");
    pp.BackBufferHeight = conf->get<int>("height");
    // TODO: D3DPRESENT_DONOTWAIT
    pp.PresentationInterval = conf->get<bool>("vertical_sync") ? D3DPRESENT_INTERVAL_ONE : 
                                                                 D3DPRESENT_INTERVAL_IMMEDIATE;
    // В полноэкранном режиме используем разрешение рабочего стола.
    // TODO: вынести в конфиг.
    if (!pp.Windowed)
    {
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFO);
        HMONITOR monitor = MonitorFromWindow(init.renderDevice->window(), MONITOR_DEFAULTTOPRIMARY);
        check_win32(GetMonitorInfo(monitor, &monitorInfo));
        
        pp.BackBufferWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        pp.BackBufferHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    }
    init.renderDevice->reset(pp);
    
    // Getting device capabilities
    check_directx(device->GetDeviceCaps(&device_caps));
    D3DDEVICE_CREATION_PARAMETERS dp;
    device->GetCreationParameters(&dp);
    if (((device_caps.PixelShaderVersion & 0xff00) >> 8) != 0 &&
        !FAILED(d3d->CheckDeviceFormat(dp.AdapterOrdinal, dp.DeviceType, D3DFMT_X8R8G8B8,
                                       0, D3DRTYPE_TEXTURE, D3DFMT_L8)))
    {
        shaders_enabled = true;
        logger()->info() << "Shader video acceleration enabled";
    }
    else
        logger()->warn() << "Pixel shader YUV conversion is not available.";
    
    // Checking if render-to-texture is available
    if (conf->get<bool>("enable_render_to_texture"))
    {
        HRESULT hr = d3d->CheckDeviceFormat(dp.AdapterOrdinal, dp.DeviceType, D3DFMT_X8R8G8B8,
                                            D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, D3DRTYPE_TEXTURE,
                                            D3DFMT_A8R8G8B8);
        if (FAILED(hr))
            logger()->warn() << "Graphical device doesn't support rendering to texture.";
        else
        {
            rtt_enabled = true;
            logger()->info() << "Rendering to texture enabled.";
            
            // Obtaining default render target
            LPDIRECT3DSURFACE9 rt;
            check_directx(device->GetRenderTarget(0, &rt));
            default_render_target.push_back(rt);
        }
    }
    
    // Checking for hardware YUV conversion support
    if (!(device_caps.DevCaps2 & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES) ||
        FAILED(d3d->CheckDeviceFormatConversion(dp.AdapterOrdinal, dp.DeviceType, D3DFMT_YUY2,
                                                D3DFMT_A8R8G8B8)) ||
        FAILED(d3d->CheckDeviceFormat(dp.AdapterOrdinal, dp.DeviceType, D3DFMT_X8R8G8B8, 0,
                                      D3DRTYPE_TEXTURE, D3DFMT_YUY2)) ||
        FAILED(d3d->CheckDeviceFormat(dp.AdapterOrdinal, dp.DeviceType, D3DFMT_X8R8G8B8,
                                      D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8)))
        logger()->warn() << "Graphical device doesn't support YUV conversion.";
    else
        hardware_yuv_enabled = true;
    
    if (!(device_caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES))
        logger()->warn() << "Graphical device doesn't support dynamic textures";
    
    // Creating managers
    texture_manager.doCreate();
    texture_manager->init(device);
    sequence_manager.doCreate();
    effects_manager.doCreate();

    // Creating text renderer
    check_directx(D3DXCreateFontA(device, 15, 0, 0, 0, 0, 0, 
                                  0, 0, 0, "Courier", &text_drawer));
    
    // Creating matrix stack
    check_directx(D3DXCreateMatrixStack(0, &matrix_stack));
    
    // Direct3D miscellaneous initialization
    setup_renderstate();
}

// Destructor
Direct3DInstance::~Direct3DInstance()
{
#define SAFE_RELEASE(res) { if (res) res->Release(); }

    SAFE_RELEASE(text_drawer);
    // Releasing Direct3D resources
    effects_manager.~effects_manager();
    sequence_manager.~sequence_manager();
    texture_manager.~texture_manager();
    
    // Releasing Direct3D instances
    for (std::vector<LPDIRECT3DSURFACE9>::iterator i = default_render_target.begin();
         default_render_target.end() != i; ++i)
        SAFE_RELEASE((*i));
    SAFE_RELEASE(matrix_stack);
#undef SAFE_RELEASE
}

// Updating graphics system
void Direct3DInstance::update( DWORD dt )
{
    texture_manager->update(dt);
    //vertex_manager->update(dt);
    sequence_manager->doUpdate(dt);
}

// Clear screen
void Direct3DInstance::clear( D3DCOLOR color )
{
    ASSERT(device);
    
    // Сохраняем предыдущие параметры.
    D3DVIEWPORT9 oldViewport;
    check_directx(device->GetViewport(&oldViewport));
    
    // Устанавливаем viewport во весь экран.
    D3DPRESENT_PARAMETERS params = utility::singleton<render::Device>()->parameters();
    D3DVIEWPORT9 vp;
    vp.X = 0;
    vp.Y = 0;
    vp.Width = params.BackBufferWidth;
    vp.Height = params.BackBufferHeight;
    vp.MinZ = 0;
    vp.MaxZ = 1;
    check_directx(device->SetViewport(&vp));
    
    // Очищаем экран.
    color = 0;
    check_directx(device->Clear(NULL, NULL, D3DCLEAR_TARGET, color, 1.0f, 0));
    
    // Возвращаем старые параметры.
    check_directx(device->SetViewport(&oldViewport));
}

// Saving screenshot
void Direct3DInstance::save_screenshot( char * filename, int width, int height )
{
    if(this->need_save_screenshot)
        return;

    this->need_save_screenshot = true;
    screenshot_name = filename;
    screenshot_width = width;
    screenshot_height = height;
}

// Present all geometry and flip buffers 
void Direct3DInstance::present()
{

    if (need_save_screenshot)
    {
        Direct3D d3d;
        LPDIRECT3DSURFACE9 small_surface, old_surface;


// Macro for creating texture with E_OUTOFMEMORY handling
#define DIRECTX_MEMORY_CHECK(create_func_call)                  \
    {                                                           \
        HRESULT hr = create_func_call;                          \
        if (FAILED(hr))                                         \
        {                                                       \
            if (D3DERR_OUTOFVIDEOMEMORY == hr)                  \
            {                                                   \
                TextureManager tm;                              \
                UINT old_ptf = tm->percents_to_free;            \
                tm->percents_to_free = 100;                     \
                tm->collect();                                  \
                tm->percents_to_free = old_ptf;                 \
                hr = create_func_call;                          \
            }                                                   \
            if (FAILED(hr))                                     \
                throw_directx_error(hr, #create_func_call);     \
        }                                                       \
    }       

        DIRECTX_MEMORY_CHECK(d3d->device->CreateRenderTarget(screenshot_width, screenshot_height, 
                                                       D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 0, 
                                                       FALSE, &small_surface, NULL));

        D3DVIEWPORT9 oldViewport;
        check_directx(d3d->device->GetViewport(&oldViewport));
        check_directx(d3d->device->GetRenderTarget(0, &old_surface));
        check_directx(d3d->device->SetRenderTarget(0, small_surface));

        check_directx(device->BeginScene());
        TRY(sequence_manager->renderQueue());
        check_directx(device->EndScene());

        check_directx(d3d->device->SetRenderTarget(0, old_surface));
        check_directx(d3d->device->SetViewport(&oldViewport));
        old_surface->Release();

        DIRECTX_MEMORY_CHECK(D3DXSaveSurfaceToFileA(screenshot_name.c_str(), D3DXIFF_PNG, small_surface, NULL, NULL));        

#undef DIRECTX_MEMORY_CHECK

        small_surface->Release();
        
        need_save_screenshot = false;
    }
    
    check_directx(device->BeginScene());
    TRY(sequence_manager->flushQueue());
    check_directx(device->EndScene());
    
    // Presenting scene
    HRESULT result = device->Present(NULL, NULL, NULL, NULL);
    if (D3DERR_DEVICELOST != result)
    {
        check_directx(result);
    }
    else
        is_device_lost = true;
}

// Direct3D matrix setup
void Direct3DInstance::setup_matrices()
{
    D3DXMatrixTranslation(&matrix_view, -0.5, -0.5, 5);
    const float w = 1024, h = 768;

    D3DXMatrixOrthoOffCenterLH(&matrix_projection, 0, w, h, 0, 
                               float(ZNear), float(ZFar));
    check_directx(device->SetTransform(D3DTS_VIEW, &matrix_view));
    check_directx(device->SetTransform(D3DTS_PROJECTION, &matrix_projection));
}

// Setup Direct3D rendering state
void Direct3DInstance::setup_renderstate()
{
    // Turning off z-testing (we will render objects in correct order)
    check_directx(device->SetRenderState(D3DRS_ZENABLE, FALSE));
    
    // Enabling culling (counter-clockwise polygons will be culled)
    check_directx(device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    
    // Alpha testing for transparent backgrounds
    check_directx(device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE)); 
    check_directx(device->SetRenderState(D3DRS_ALPHAREF, 1));
    check_directx(device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL));
    
    // Enabling blending texture and material color
    check_directx(device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE));
    
    // Setting texture filters
    check_directx(device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
    check_directx(device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
    
    // Texture addressing mode
    device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    
    // Enabling alpha-blending
    check_directx(device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
    check_directx(device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
    check_directx(device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA)); 
    
    check_directx(device->SetVertexShader(NULL));

    // Setting up lighting but not enabling it (we'll turn it off when
    // we really need it)
    check_directx(device->SetRenderState(D3DRS_LIGHTING, FALSE));
    
    // Default light
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(light));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 1.0f;
    light.Diffuse.g = 1.0f;
    light.Diffuse.b = 1.0f;
    light.Diffuse.a = 1.0f;

    light.Specular = light.Diffuse;
    light.Ambient = light.Diffuse;

    light.Direction.x = 0.0f;
    light.Direction.y = 0.0f;
    light.Direction.z = 1.0f;
    light.Range = ZFar;
    
    check_directx(device->SetLight(0, &light));
    check_directx(device->LightEnable(0, TRUE));
    check_directx(device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, 
                                          D3DMCS_MATERIAL));

    // Default material
    D3DMATERIAL9 mtrl;
    ZeroMemory(&mtrl, sizeof(mtrl));
    D3DCOLORVALUE color = {1, 1, 1, 1};
    mtrl.Diffuse = mtrl.Ambient = mtrl.Specular = color;
    check_directx(device->SetMaterial(&mtrl));
    
    // Disabling vertex shader
    check_directx(device->SetVertexShader(NULL));
    
    // Setting transformation matrices
    setup_matrices();
}

// Get current transformation matrix
D3DXMATRIX Direct3DInstance::get_transform()
{
    return *matrix_stack->GetTop();
}

// Push transformation to stack
void Direct3DInstance::push_transform( const D3DXMATRIX & new_transform )
{
    matrix_stack->Push();
    matrix_stack->MultMatrixLocal(&new_transform);
}

// Pop transformation from stack
void Direct3DInstance::pop_transform()
{
    matrix_stack->Pop();
}

// Clear rendering queue
void Direct3DInstance::clear_render_queue()
{
    sequence_manager->clearQueue();
    matrix_stack->Release();
    check_directx(D3DXCreateMatrixStack(0, &matrix_stack));
}

// Check if device is lost and can be restored
bool Direct3DInstance::is_lost()
{
    if (!is_device_lost)
        return false;
    
    // Checking if device can be reset
    HRESULT hr = device->TestCooperativeLevel();
    switch (hr)
    {
    // Device is lost and cannot be restored
    case D3DERR_DEVICELOST:
        return true;
    
    // Device should be restored immediately
    case D3DERR_DEVICENOTRESET:
    
        // Releasing resources
        if (default_render_target.size() > 0)
        {
            ASSERT(default_render_target.size() == 1);
            default_render_target[0]->Release();
            default_render_target.clear();
        }
        
        texture_manager->unload_textures();
        effects_manager->release_resources();
        text_drawer->OnLostDevice();

        //VertexManagerInstance::VertexBufferInstance::force_update = true;
        {
            utility::singleton<render::Device> renderDevice;
            renderDevice->reset(renderDevice->parameters());
        }
        
        // Restoring resources

        if (rtt_enabled)
        {
            // Obtaining default render target
            LPDIRECT3DSURFACE9 rt;
            check_directx(device->GetRenderTarget(0, &rt));
            default_render_target.push_back(rt);
        }

        text_drawer->OnResetDevice();
        effects_manager->restore_resources();
        
        setup_renderstate();
        is_device_lost = false;
        return true;
    }
    // Unknown error
    throw_directx_error(hr, "Device reset failed");
}
