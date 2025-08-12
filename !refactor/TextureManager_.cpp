#include <precompiled.h>
#include "TextureManager_.h"
#include "Application_.h"
#include <tanita2/application/application.h>

#define D3D_TM Direct3DInstance::TextureManagerInstance

// Available texture memory
unsigned int D3D_TM::available_mem;
// Utilized texture memory
unsigned int D3D_TM::utilized_mem;
// Number of percents of video memory to free during collection
unsigned int D3D_TM::percents_to_free;

// Constructor
D3D_TM::TextureInstance::TextureInstance( const ResourceId & path, bool compressed_ )
    : texture(NULL), compressed(compressed_)
{
    ZeroMemory(&texture_desc, sizeof(DDSURFACEDESC2));
    
    // Loading file
    ResourceManagerSingleton fm;
    ResourceId p(path);
    TRY(file = fm->loadResource(p));
    
    // Obtaining texture info
    const U8 * dds = reinterpret_cast<U8 *>(file->getStream(RESOURCE_STREAM_DEFAULT));
    // DDS signature
    ASSERT(*(DWORD *)dds == (('D' << 0) | ('D' << 8) | ('S' << 16) | (' ' << 24)));
    CopyMemory(&texture_desc, dds + 4, sizeof(texture_desc));
    ASSERT(0 != (texture_desc.dwReserved & 0xFFFF) && 0 != (texture_desc.dwReserved >> 16));
    gc_info.size = texture_desc.dwWidth * texture_desc.dwHeight * 4 / 1024;
    compressed = (texture_desc.ddpfPixelFormat.dwFlags & DDPF_FOURCC);
    if (compressed)
        gc_info.size /= 4;
}

// Macro for creating texture with E_OUTOFMEMORY handling
#define CREATE_TEXTURE(create_func_call)                                                             \
    {                                                                                                \
        HRESULT hr = create_func_call;                                                               \
        if (FAILED(hr))                                                                              \
        {                                                                                            \
            if (D3DERR_OUTOFVIDEOMEMORY == hr)                                                       \
            {                                                                                        \
                utility::singleton<TextureManagerInstance> tm;                                       \
                UINT old_ptf = percents_to_free;                                                     \
                percents_to_free = 100;                                                              \
                tm->collect();                                                                       \
                percents_to_free = old_ptf;                                                          \
                hr = create_func_call;                                                               \
            }                                                                                        \
            if (FAILED(hr))                                                                          \
            {                                                                                        \
                if (D3DERR_OUTOFVIDEOMEMORY == hr)                                                   \
                {                                                                                    \
                     MessageBoxA(application::Application::window(),                                 \
                         "Игре не хватает памяти. Запущено слишком много приложений,"                \
                         " либо Ваш компьютер не соответствует минимальным требованиям игры.",       \
                         "Ошибка", MB_ICONERROR | MB_OK);                                            \
                     exit(0);                                                                        \
                }                                                                                    \
                throw_directx_error(hr, #create_func_call);                                       \
            }                                                                                        \
        }                                                                                            \
    }

// Load texture to Direct3D
bool D3D_TM::TextureInstance::load()
{
    if (texture) return false;
    Direct3D d3d;
    
    // Какой идиот из M$ сделал первой высоту, а не ширину картинки???
    const U8 * data = reinterpret_cast<U8 *>(file->getStream(RESOURCE_STREAM_DEFAULT)); 
    DWORD w = *((DWORD *)data + 4);
    DWORD h = *((DWORD *)data + 3);
    
    
    // Creating and filling off-screen texture
    D3DFORMAT fmt = compressed ? D3DFMT_DXT3 : D3DFMT_A8R8G8B8;
    LPDIRECT3DTEXTURE9 offscreen;
    CREATE_TEXTURE(d3d->device->CreateTexture(w, h, 1, 0, fmt, D3DPOOL_SYSTEMMEM, &offscreen, NULL));
    D3DLOCKED_RECT lr;
    check_directx(offscreen->LockRect(0, &lr, NULL, D3DLOCK_NO_DIRTY_UPDATE));
    
    BYTE * ptr = (BYTE *)data + 128;
    int heightCount = compressed ?
                        (h >> 2) :
                         h;
    if ((w << 2) == lr.Pitch)
        memcpy((BYTE *)lr.pBits, ptr, lr.Pitch * heightCount);
    else
        for (int y = 0; y < heightCount; ++y)
            memcpy((BYTE *)lr.pBits + y * lr.Pitch, ptr + (w << 2), (w << 2));
    check_directx(offscreen->UnlockRect(0));
    
    /*
    // Код для определения используемой площади текстуры.
    if (compressed)
    {
        int top = h - 1, bottom = 0, left = w - 1, right = 0;
        for (int y = 0; y < (h >> 2); ++y)
            for (int x = 0; x < (w >> 2); ++x)
            {
                // Альфа-блок непрозрачен.
                ULONG64 null = 0;
                if (0 != memcmp(ptr + ((w << 2) * y + (x << 4)), &null, 8))
                {
                    if (left > x)
                        left = x;
                    if (right < x)
                        right = x;
                    if (top > y)
                        top = y;
                    if (bottom < y)
                        bottom = y;
                }
            }
        
        DWORD full = w * h;
        DWORD used = (((right - left + 1) * (bottom - top + 1)) << 4);
        gFullKpx += (full >> 10);
        gUsedKpx += (used >> 10);
        
        if (0 != full && 100 * used / full < 70)
            logger()->profile() << "Texture " << path.getFilePath() << " uses "
                                << (100 * used / full) << "% of texture space";
    }
    */
    
    // Creating and updating texture
    CREATE_TEXTURE(d3d->device->CreateTexture(w, h, 1, 0, fmt, D3DPOOL_DEFAULT, &texture, NULL));
    check_directx(d3d->device->UpdateTexture(offscreen, texture));
    offscreen->Release();
    
    TextureManagerInstance::utilized_mem += gc_info.size;
    return true;
}

// Unloading texture from video memory
void D3D_TM::TextureInstance::unload()
{
    if (!texture) return;
    texture->Release();
    texture = NULL;
    
    TextureManagerInstance::utilized_mem -= gc_info.size;
}

// Bind texture to Direct3D
void D3D_TM::TextureInstance::bind( int sampler_index )
{
    load();
    gc_info.age = 0;
    Direct3D d3d;
    check_directx(d3d->device->SetTexture(sampler_index, texture));
}

// Texture loading
D3D_TM::TextureRef D3D_TM::load( const ResourceId & path, bool compressed )
{
    // Checking if texture was loaded before
    if (!path.isNonPackable())//!(path.getFlags() & FM_FLAG_NO_CACHE))
        for (TextureIter i = textures.begin(); textures.end() != i; ++i)
            if (!(*i)->path.isNonPackable() && (*i)->path == path)
                return *i;
    
    DeprecatedApplicationInstance::pause();
    textures.push_back(TextureRef(new TextureInstance(path, compressed)));
    TextureRef & t = textures.back();
    t->path.setFromResourceId(path);
    DeprecatedApplicationInstance::resume();
    return t;
}

// Constructor
D3D_TM::DynamicTextureInstance::DynamicTextureInstance( int width, int height, bool compressed )
    : width(width), height(height)
{
    this->compressed = compressed;
    
    ZeroMemory(&texture_desc, sizeof(DDSURFACEDESC2));
    texture_desc.dwWidth = width;
    texture_desc.dwHeight = height;
    gc_info.size = texture_desc.dwWidth * texture_desc.dwHeight * 4 / 1024;
    if (compressed)
        gc_info.size /= 4;
    gc_info.is_dynamic = true;
}

// Create Direct3D texture
bool D3D_TM::DynamicTextureInstance::load()
{
    if (texture) return false;
    Direct3D d3d;

    // Some Intel cards don't support dynamic textures, using managed instead
    static D3DPOOL pool = D3DPOOL_DEFAULT;
    static DWORD usage = (d3d->device_caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) ? 
                              // Device supports dynamic textures
                              D3DUSAGE_DYNAMIC :
                              // Device doesn't support: using managed pool
                              (pool = D3DPOOL_MANAGED, 0);
    CREATE_TEXTURE(d3d->device->CreateTexture(width, height, 1, usage,
                       compressed ? D3DFMT_DXT3 : D3DFMT_A8R8G8B8, 
                       pool, &texture, NULL));

    TextureManagerInstance::utilized_mem += gc_info.size;
    return true;
}

// Constructor
D3D_TM::SurfaceTextureInstance::SurfaceTextureInstance( int width, int height, 
                                                        D3DFORMAT format, bool render_target )
    : width(width), height(height), format(format), 
      surface(NULL), render_target(render_target)
{
    this->compressed = false;
    ZeroMemory(&texture_desc, sizeof(DDSURFACEDESC2));
    texture_desc.dwWidth = width;
    texture_desc.dwHeight = height;
    
    gc_info.size = texture_desc.dwWidth * texture_desc.dwHeight * 4 / 1024;
    gc_info.is_dynamic = false;
}

// Create texture
bool D3D_TM::SurfaceTextureInstance::load()
{
    if (texture) return false;
    
    Direct3D d3d;
    static DWORD usage = (d3d->device_caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) ? D3DUSAGE_DYNAMIC : 0;
    const DWORD real_usage = (render_target ? D3DUSAGE_RENDERTARGET : usage);
    CREATE_TEXTURE(d3d->device->CreateTexture(width, height, 1, real_usage, format, 
                                              D3DPOOL_DEFAULT, &texture, NULL));
    // Obtaining surface
    check_directx(texture->GetSurfaceLevel(0, &surface));

    // Updating gc info
    TextureManagerInstance::utilized_mem += gc_info.size;
    return true;
}

// Release resources
void D3D_TM::SurfaceTextureInstance::unload()
{
    if (surface)
    {
        surface->Release();
        surface = NULL;
    }
    TextureInstance::unload();
}

// Constructor
D3D_TM::RenderTargetTextureInstance::RenderTargetTextureInstance()
    : width(0), height(0)
{
    // Determining width and height
    Direct3D d3d;
    D3DPRESENT_PARAMETERS params = utility::singleton<render::Device>()->parameters();
    width = params.BackBufferWidth;
    height = params.BackBufferHeight;
    if ((d3d->device_caps.TextureCaps & D3DPTEXTURECAPS_POW2) &&
        !(d3d->device_caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL))
    {
        int w = 256;
        while (w < width)
            w <<= 1;
        width = height = w;
    }

    ZeroMemory(&texture_desc, sizeof(DDSURFACEDESC2));
    texture_desc.dwWidth = width;
    texture_desc.dwHeight = height;
    gc_info.size = texture_desc.dwWidth * texture_desc.dwHeight * 4 / 1024;
}

// Create render target texture
bool D3D_TM::RenderTargetTextureInstance::load()
{
    if (texture) return false;
    Direct3D d3d;
    CREATE_TEXTURE(d3d->device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET,
                                              D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL));
    TextureManagerInstance::utilized_mem += gc_info.size;
    return true;
}
#undef CREATE_TEXTURE

// Create texture for dynamic use
D3D_TM::TextureRef D3D_TM::create_dynamic( int width, int height,
                                           DynamicTextureInstance ** texture_ptr,
                                           bool compressed )
{
    ASSERT(NULL != texture_ptr);
    *texture_ptr = new DynamicTextureInstance(width, height, compressed);
    textures.push_back(TextureRef(*texture_ptr));
    return textures.back();
}

// Create render target texture
D3D_TM::TextureRef D3D_TM::create_render_target()
{
    textures.push_back(TextureRef(new RenderTargetTextureInstance()));
    return textures.back();
}

// Create texture with surface
D3D_TM::TextureRef D3D_TM::create_surface_texture( int width, int height, D3DFORMAT format,
                                                   SurfaceTextureInstance ** texture_ptr,
                                                   bool render_target )
{
    ASSERT(NULL != texture_ptr);
    *texture_ptr = new SurfaceTextureInstance(width, height, format, render_target);
    textures.push_back(TextureRef(*texture_ptr));
    return textures.back();
}

// Initialization
void D3D_TM::init( LPDIRECT3DDEVICE9 device )
{
    Config config;
    try { percents_to_free = (UINT)config->get<int>("vmem_hyst"); }
    catch (...) { percents_to_free = 20; }
    
    available_mem = 0;
    try { int vmem_limit = (UINT)config->get<int>("video_memory_limit");
          if (0 < vmem_limit) available_mem = vmem_limit; }
    catch (...) { percents_to_free = 20; }
    
    if (0 == available_mem)
        available_mem = device->GetAvailableTextureMem() / 1024;
    
    logger()->info() << "Available video memory: " << (available_mem / 1024) << "Mb";
}

// Free unused texture memory
void D3D_TM::collect( bool low_sysmem_hint )
{
#ifndef _DEBUG_TEXTURE_COLLECTOR
    // Updating available memory count
    if (!low_sysmem_hint && utilized_mem < available_mem)
    {
        logger()->debug() << "Updating texture gc available memory size: "
                          << (utilized_mem / 1024) << "MB instead of "
                          << (available_mem / 1024) << "MB";
        available_mem = utilized_mem;
    }
#endif
    // Memory size to reach (maximum if cleaning system memory instead of video)
    UINT to_reach = low_sysmem_hint ? 0 : available_mem * (100 - percents_to_free) / 100;
    
    
    // Aggressively freeing textures from other scenes
    for (TextureIter i = textures.begin(); textures.end() > i;)
    {
        if (!i->unique())
        {
            ++i;
            continue;
        }
        
        (*i)->unload();
        i = textures.erase(i);
        
        if (utilized_mem < to_reach)
            goto print_usage;
    }
    if (low_sysmem_hint)
        goto print_usage; // We've done with cleaning unneeded system memory objects
    
    // Freeing textures that are not used currently (non-visible)
    for (TextureIter i = textures.begin(); textures.end() > i; ++i)
    {
        TextureInstance::GCInfo & info = (*i)->gc_info;
        if (info.age > 70 /* 15fps */)
            (*i)->unload();
        if (utilized_mem < to_reach) return;
    }
    
print_usage:

    return;
}

// Updating textures
void D3D_TM::update( DWORD dt )
{
    // Skipping if no actual update or if there are plenty of video mem
    if (0 == dt || utilized_mem * 100 / available_mem < 100 - percents_to_free) return;
    
    // Releasing textures that are not used in current scene
    for (TextureIter i = textures.begin(); i != textures.end();)
    {
        TextureInstance::GCInfo & info = (*i)->gc_info;
        if (i->unique() && info.age > 10000) // Ten seconds from last low memory warning
        {
            (*i)->unload();
            i = textures.erase(i);
            continue;
        }
        else
            ++i;
        info.age += dt;
    }
}

// Releasing all texture memory for device restoring
void D3D_TM::unload_textures()
{
    for (TextureIter i = textures.begin(); textures.end() > i; ++i)
        (*i)->unload();
}

#undef D3D_TM
