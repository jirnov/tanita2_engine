#include <precompiled.h>
#include "Graphics_.h"
#include "EffectsManager_.h"
#include "graphics/sequenceManager/sequenceManager.h"

#include "Python_.h"

#include "Application_.h"
#include "Sequence_.h"
#include "GameObject_.h"

#include "VideoSequence_.h"

#include "base/sync/criticalSection.h"
#include "resourceManager/resourceClasses/resourceClasses.h"
#include "resource.h"

using namespace ingame;

//! Constructor
ShaderVideoSequence::ShaderVideoSequence( char * path )
    : VideoSequenceBase(path), target_texture(NULL), effect(NULL)
{
    // Creating textures
    TextureManager tm;
    
    UINT wp2 = 1, hp2 = 1;
    while (wp2 < header->videoInfo.frame_width)  wp2 <<= 1;
    while (hp2 < header->videoInfo.frame_height) hp2 <<= 1;
    TRY(texture = tm->create_surface_texture(wp2, hp2 << 1, D3DFMT_L8, &target_texture));
    
    // Creating vertex buffer
    buffer = SequenceManagerSingleton()->create(wp2, hp2);
    
    EffectsManager em;
    effect = em->load(IDR_YUV_EFFECT);
    matrixHandle = effect->GetParameterBySemantic(NULL, "WORLDVIEWPROJ");
    textureHandle = effect->GetParameterBySemantic(NULL, "TEXTURE");
    textureSizeHandle = effect->GetParameterBySemantic(NULL, "TEXTURESIZE");
    videoSizeHandle = effect->GetParameterBySemantic(NULL, "VIDEOSIZE");
}

// Sequence rendering
void ShaderVideoSequence::doRender()
{
    Direct3D d3d;
    d3d->device->SetTransform(D3DTS_WORLD, &mTransformationMatrix);

    if (target_texture->load()) // load() returns true if texture was really loaded during call
        fill_texture();
    
    check_directx(effect->SetMatrix(matrixHandle, &(mTransformationMatrix * d3d->get_view_matrix() * d3d->get_projection_matrix())));
    check_directx(effect->SetTexture(textureHandle, texture->get_texture()));
    D3DXVECTOR4 tsize(float(target_texture->width), float(target_texture->height), 0, 0);
    check_directx(effect->SetVector(textureSizeHandle, &tsize));
    D3DXVECTOR4 vsize(float(header->videoInfo.frame_width), float(header->videoInfo.frame_height), 0, 0);
    check_directx(effect->SetVector(videoSizeHandle, &vsize));
    
    UINT passes;
    check_directx(effect->Begin(&passes, 0));
    for (UINT pass = 0; pass < passes; ++pass)
    {
        check_directx(effect->BeginPass(pass));
        TRY(texture->bind(0));
        //TRY(texture->bind(1));
        //TRY(texture->bind(2));
        TRY(buffer->draw());
        check_directx(effect->EndPass());
        check_directx(effect->End());
    }
}

// Updating
bool ShaderVideoSequence::onAddingToQueue( float dt )
{
    bool result = VideoSequenceBase::onAddingToQueue(dt);
    if (need_texture_fill)
        fill_texture();
    return result;
}

// Fill texture with data
void ShaderVideoSequence::fill_texture()
{
    // Creating textures (if needed)
    target_texture->load();
    
    if (need_texture_fill)
    {
        D3DLOCKED_RECT lr;
        check_directx(target_texture->surface->LockRect(&lr, NULL, D3DLOCK_DISCARD));
        
        BYTE * frame_byte = (BYTE *)lr.pBits;
        const int width  = header->videoInfo.frame_width,
                  height = header->videoInfo.frame_height,
                  y_stride = video_yuv_buffer[0].stride,
                  uv_stride = video_yuv_buffer[1].stride;
        const BYTE * y = video_yuv_buffer[0].data,
                   * u = video_yuv_buffer[1].data,
                   * v = video_yuv_buffer[2].data;
        for (int i = 0; i < height; ++i)
        {
            memcpy(frame_byte, y, width);
            y += y_stride;
            frame_byte += lr.Pitch;
        }
        BYTE * v_dst = frame_byte + (height >> 1) * lr.Pitch;
        for (int i = 0; i < (height >> 1); ++i)
        {
            memcpy(frame_byte, u, width >> 1);
            memcpy(v_dst, v, width >> 1);
            u += uv_stride;
            v += uv_stride;
            v_dst += lr.Pitch;
            frame_byte += lr.Pitch;
        }
        check_directx(target_texture->surface->UnlockRect());
    }
}
