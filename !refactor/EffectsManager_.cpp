#include <precompiled.h>
#include "EffectsManager_.h"

#define D3D_EM Direct3DInstance::EffectsManagerInstance

// Constructor
D3D_EM::EffectsManagerInstance()
    : pool(NULL)
{
    check_directx(D3DXCreateEffectPool(&pool));
}

// Python exception text string for shader error log
namespace internals
{
    extern std::string traceback;
}

// Load effect
EffectRef D3D_EM::load( INT resource )
{
    Direct3D d3d;
    if (!d3d->rtt_enabled)
        return NULL;
    
    // Effect flag
    const int SHADER_FLAGS = 0; // D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION

    // Creating effect
    LPD3DXBUFFER errors;
    LPD3DXEFFECT effect;
    HRESULT result = D3DXCreateEffectFromResource(d3d->device, NULL, MAKEINTRESOURCE(resource), NULL,
                         NULL, SHADER_FLAGS, pool, &effect, &errors);
    if (FAILED(result))
    {
        std::string err = boost::str(boost::format("Effect loading failed with error message: %1%") %
                                     (char *)errors->GetBufferPointer());
        internals::traceback += err + "\n";
        logger()->error() << err;
        errors->Release();
        check_directx(result);
    }
    // Adding effect to list of effects
    effects.push_back(std::pair<std::string, EffectRef>("effect", effect));
    return effect;
}

// Release resources before device reset
void D3D_EM::release_resources()
{
    // Releasing effects
    for (EffectsIter i = effects.begin(); effects.end() != i; ++i)
        i->second->OnLostDevice();
}

// Restore resources after device reset
void D3D_EM::restore_resources()
{
    // Releasing effects
    for (EffectsIter i = effects.begin(); effects.end() != i; ++i)
        i->second->OnResetDevice();
}

// Destructor
D3D_EM::~EffectsManagerInstance()
{
    // Releasing effects
    for (EffectsIter i = effects.begin(); effects.end() != i; ++i)
        i->second->Release();

    // Releasing effects pool
    if (pool) pool->Release();
}

#undef D3D_EM
/*
using namespace ingame;

BeginRenderTargetSequence::BeginRenderTargetSequence()
{
    // Creating render-to-target texture
    TextureManager tm;
    TRY(texture = tm->create_render_target());
    DDSURFACEDESC2 & d = texture->get_description();
    size = D3DXVECTOR4((float)d.dwWidth, (float)d.dwHeight, 0, 0);
}

// Rendering
void BeginRenderTargetSequence::doRender()
{
    // Setting render target
    LPDIRECT3DSURFACE9 surface;
    Direct3D d3d;
    texture->load();
    check_directx(texture->get_texture()->GetSurfaceLevel(0, &surface));
    check_directx(d3d->device->SetRenderTarget(0, surface));
    check_directx(d3d->device->Clear(NULL, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0));
    d3d->default_render_target.push_back(surface);
    surface->Release();
}

#pragma warning(disable:4702)
EndRenderTargetSequence::EndRenderTargetSequence( const graphics::SequenceRef & s, EffectRef effect,
                                                  D3DXHANDLE param_texture )
    : effect(effect), param_texture(param_texture)
{
    throw Exception("");
}
#pragma warning(default:4702)

// Rendering
void EndRenderTargetSequence::doRender()
{
    Direct3D d3d;
    d3d->default_render_target.pop_back();
    check_directx(d3d->device->SetRenderTarget(0, d3d->default_render_target.back()));

    D3DXMatrixIdentity(&mTransformationMatrix);
    d3d->device->SetTransform(D3DTS_WORLD, &mTransformationMatrix);
    TRY(texture->bind());
    if (param_texture) effect->SetTexture(param_texture, texture->get_texture());
    
    UINT passes;
    check_directx(effect->Begin(&passes, 0));
    for (UINT i = 0; i < passes; ++i)
    {
        check_directx(effect->BeginPass(i));
        TRY(vbuffer->render());
        check_directx(effect->EndPass());
    }
    check_directx(effect->End());
}
*/