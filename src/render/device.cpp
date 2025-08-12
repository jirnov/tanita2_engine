/*
 ������ "������2".
 ���������� ���������� �������.
 */
#include <precompiled.h>
#include <tanita2/render/device.h>
#include <tanita2/render/scene.h>
#include <tanita2/render/texture/texture.h>
#include <tanita2/render/vb/vb.h>
#include <tanita2/application/application.h>
#include <tanita2/config/config.h>
#include "window.h"
#include <deprecated/Graphics_.h>

namespace
{

// TODO: ������ ��� ������� :)
std::auto_ptr<utility::manual_singleton<Direct3DInstance> > gDeprecatedGfx;


// ����� � ��� ���������� � ����������.
void printVideoSystemInfo( LPDIRECT3D9 direct3D, UINT adapter, const D3DCAPS9 & deviceCaps )
{
    assert(NULL != direct3D);
    
    // ������������� DirectDraw ��� ��������� ���������� � �����������.
    LPDIRECTDRAW genericDirectDraw;
    LPDIRECTDRAW7 directDraw;
    check_directx(DirectDrawCreate(NULL, &genericDirectDraw, NULL));
    check_directx(genericDirectDraw->QueryInterface(IID_IDirectDraw7,
                      reinterpret_cast<void **>(&directDraw)));
    
    // ��������� ���������� � ��������� �����������.
    DDSCAPS2 caps;
    ULONG videoMemoryLocal;
    memset(&caps, 0, sizeof(DDSCAPS2));
    caps.dwCaps = DDSCAPS_LOCALVIDMEM;
    check_directx(directDraw->GetAvailableVidMem(&caps, &videoMemoryLocal, NULL));
    
    // ��������� ���������� � ������ ������ �����������.
    ULONG videoMemoryTotal;
    memset(&caps, 0, sizeof(DDSCAPS2));
    caps.dwCaps = DDSCAPS_TEXTURE;
    check_directx(directDraw->GetAvailableVidMem(&caps, &videoMemoryTotal, NULL));
    
    // ������.
    directDraw->Release();
    genericDirectDraw->Release();
    
    // ������� ���������� �� ����������.
    D3DADAPTER_IDENTIFIER9 deviceId;
    check_directx(direct3D->GetAdapterIdentifier(adapter,
                                                 0 /* WHQL level */, &deviceId));
    logger()->info() << "Video adapter: " << deviceId.Description << ".";
    logger()->info() << "Video memory: " << (videoMemoryTotal / 1048576) << "Mb total, "
                     << (videoMemoryLocal / 1048576) << "Mb onboard.";
    logger()->info() << "Shader model: vertex: "
                     << ((deviceCaps.VertexShaderVersion & 0xff00) >> 8)
                     << "." << (deviceCaps.VertexShaderVersion & 0xff) << ", pixel: "
                     << ((deviceCaps.PixelShaderVersion & 0xff00) >> 8) << "."
                     << (deviceCaps.PixelShaderVersion & 0xff) << ".";
}

// �������������� ����������� ������� � ������.
std::string textureFormatName( D3DFORMAT format )
{
    switch (format)
    {
#define TEXTURE_FORMAT_CASE(fmt) case D3DFMT_##fmt: return #fmt
    
    TEXTURE_FORMAT_CASE(X8R8G8B8);
    TEXTURE_FORMAT_CASE(A8R8G8B8);
    TEXTURE_FORMAT_CASE(A8B8G8R8);
    TEXTURE_FORMAT_CASE(X8B8G8R8);
    TEXTURE_FORMAT_CASE(R5G6B5);
    
#undef TEXTURE_FORMAT_CASE
    }
    std::stringstream name;
    name << "Unknown (0x" << std::hex << format << ')';
    return name.str();
}

} // namespace


// ��������� ���� ����������.
HWND render::Device::window()
{
    return utility::singleton<Window>::exists() ?
               utility::singleton<Window>()->getHWND() :
               NULL;
}

// �����������.
render::Device::Device()
    : mDirect3D(NULL), mDevice(NULL),
      mAdapter(D3DADAPTER_DEFAULT), mDeviceType(D3DDEVTYPE_HAL)
{
    // ������� ���� ����������.
    mWindow.doCreate();
    
    // �������� ������ DirectX � �������������.
    mDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (NULL == mDirect3D)
    {
        // TODO: �����������.
        logger()->critical() << "Direct3D initialization failed. Probably DirectX is not installed.";
        application::ShowCriticalErrorDialog(L"���������� ���������������� Direct3D. "
                                             L"��������, DirectX 9.0c �� ����������.");
        throw std::runtime_error("DirectX initialization failed.");
    }
    
    // ��������� �� ���������. ����� ���������� ��������� �� �� ��������.
    mParameters.BackBufferWidth = 0;
    mParameters.BackBufferHeight = 0;
    mParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    mParameters.BackBufferCount = 1;
    mParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
    mParameters.MultiSampleQuality = 0;
    mParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    mParameters.hDeviceWindow = window();
    mParameters.Windowed = true;
    mParameters.EnableAutoDepthStencil = false;
    mParameters.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
    mParameters.Flags = 0; // D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL; �� ������� :)
    mParameters.FullScreen_RefreshRateInHz = 0;
    mParameters.PresentationInterval = D3DPRESENT_DONOTWAIT;
    
    // ��������� ��������� ���������� ������������� ������.
    D3DCAPS9 deviceCaps;
    check_directx(mDirect3D->GetDeviceCaps(mAdapter, mDeviceType, &deviceCaps));
    DWORD vertexProcessingMode = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    if (0 == (deviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ||
        deviceCaps.VertexShaderVersion < D3DVS_VERSION(1, 1))
        vertexProcessingMode = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    
    // ���������� � ��� ���������� �� ����������.
    printVideoSystemInfo(mDirect3D, mAdapter, deviceCaps);
    
    // ���� �������� ��������� NV PerfHUD, ���� ���������� �������.
    if (config::gfx::perfHUDEnabled())
    {
        logger()->warn() << "NVPerfHUD support enabled.";
        UINT adapterCount = mDirect3D->GetAdapterCount();
        for (UINT adapter = 0; adapter < adapterCount; ++adapter)
        {
            D3DADAPTER_IDENTIFIER9 identifier;
            if (SUCCEEDED(mDirect3D->GetAdapterIdentifier(adapter, 0 /* WHQL level */,
                                                          &identifier)) &&
                NULL != (strstr(identifier.Description, "PerfHUD")))
            {
                mAdapter = adapter;
                mDeviceType = D3DDEVTYPE_REF;
                logger()->warn() << "Using NVPerfHUD device.";
                break;
            }
        }
        if (D3DADAPTER_DEFAULT == mAdapter)
            logger()->warn() << "NVPerfHUD adapter not found.";
    }
    
    // ������� ����������.
    logger()->info() << "Initializing Direct3D device.";
    check_directx(mDirect3D->CreateDevice(mAdapter, mDeviceType, window(),
                                          vertexProcessingMode, &mParameters, &mDevice));
    logger()->info() << "Back buffer: " << mParameters.BackBufferWidth
                     << 'x' << mParameters.BackBufferHeight << ". Format: "
                     << textureFormatName(mParameters.BackBufferFormat);
    
    // ��������� ����������� ���������� ����������.
    if (FAILED(mDirect3D->CheckDeviceFormat(mAdapter, mDeviceType,
                   mParameters.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT3)))
    {
        // TODO: �����������.
        logger()->critical() << "DXT3 texture format is not supported.";
        application::ShowCriticalErrorDialog(L"������������ �� ������������ ���������� ������ DXT3. ");
        throw std::runtime_error("DXT3 texture compression is not supported.");
    }
    
    // ������� ���������� ��������.
    logger()->info() << "Initializing texture manager.";
    TextureManager::initializer initializer;
    initializer.maxSystemMemory = 0; // TODO: ������ ��������.
    initializer.maxVideoMemory = 0;
    mTextureManager.doCreate(initializer);
    
    // ������� ��������� ��������.
    logger()->info() << "Initializing vertex buffer manager.";
    mVertexBufferManager.doCreate();
    
    // ������� ���� �����.
    logger()->info() << "Initializing scene graph.";
    mSceneGraph.doCreate();
    
    // TODO: ������, ����� �� ��������� ��������� D3DCAPS2_DYNAMICTEXTURES.
    // TODO: ����� ������� ��������� �����.
    // TODO: ����� �� render to texture?
    
    // ������-������� ������� ������.
    gDeprecatedGfx.reset(new utility::manual_singleton<Direct3DInstance>());
    Direct3DInstance::initializer i;
    i.d3d = mDirect3D;
    i.device = mDevice;
    i.renderDevice = this;
    i.params = mParameters;
    gDeprecatedGfx->doCreate(i);
}

// ����������.
render::Device::~Device()
{
    // ������� ������ ������.
    gDeprecatedGfx.reset();
    
    if (NULL != mDevice)
        mDevice->Release();
    if (NULL != mDirect3D)
        mDirect3D->Release();
    
    logger()->info() << "Render device destroyed.";
}

// ����� ���������� � ���������� ����� ����������.
void render::Device::reset( const D3DPRESENT_PARAMETERS & newParameters )
{
    assert(NULL != mDevice);
    mParameters = newParameters;
    
    // ���������������� � ������ ����������.
    mTextureManager->prepareToReset();
    mVertexBufferManager->prepareToReset();
    
    // ��������� ��������� ����.
    u32 width = newParameters.BackBufferWidth,
        height = newParameters.BackBufferHeight;
    logger()->info() << "Updating application window with new parameters.";
    mWindow->reset(width, height, newParameters.Windowed);
    
    // ���������� ����������.
    logger()->info() << "Resetting render device with new parameters.";
    check_directx(mDevice->Reset(&mParameters));
    logger()->info() << "Render device reset succeeded.";
    logger()->info() << "Back buffer: " << width << 'x' << height << ". Format: "
                     << textureFormatName(mParameters.BackBufferFormat);
    
    // ��������������� �������.
    mVertexBufferManager->restoreAfterReset();
    mTextureManager->restoreAfterReset();
    
    // ������������� viewport.
    u32 vpWidth = width;
    if (float(width) / height > 1.4f)
    {
        vpWidth = u32(height * 4.0f / 3.0f);
        logger()->info() << "Widescreen mode. Viewport width: " << vpWidth << "px";
    }
    D3DVIEWPORT9 vp;
    vp.X = (width - vpWidth) / 2;
    vp.Y = 0;
    vp.Width = vpWidth;
    vp.Height = newParameters.BackBufferHeight;
    vp.MinZ = 0;
    vp.MaxZ = 1;
    check_directx(mDevice->SetViewport(&vp));
    
    logger()->info() << "Device reset complete.";
}

// �������, ���������� �� ������ ����� ��� �������� �������������� ��������.
void render::Device::onFrame( u32 /* dt */ )
{
    mVertexBufferManager->unloadUnusedResources();
    mTextureManager->unloadUnusedResources();
}
