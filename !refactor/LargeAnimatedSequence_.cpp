#include <precompiled.h>
#include "LargeAnimatedSequence_.h"
#include "Application_.h"

using namespace ingame;

// Creating sequence for given path and texture count
LargeAnimatedSequence::LargeAnimatedSequence( const DirectoryId & path, 
                                              std::vector<int> & frame_indices, bool compressed )
    : AnimatedSequenceBase(frame_indices), lowlevel_texture(NULL), loaded_frame(-1)
{
    ResourceManagerSingleton fm;
    bool compressionFlagSetup = false;
    for (int j = 0; j < (int)frame_indices.size(); ++j)
    {
        ResourceRef f;
        int i = frame_indices[j];
        TRY(f = fm->loadResource(path.toResourceId(i)));
        
        const U8 * data = reinterpret_cast<U8 *>(f->getStream(RESOURCE_STREAM_DEFAULT));
        width = *((DWORD *)data + 4);
        height = *((DWORD *)data + 3);

        const U8 * dds = data;
        mBoundingBox.x = float(((DDSURFACEDESC2 *)(dds + 4))->dwReserved & 0xFFFF);
        mBoundingBox.y = float(((DDSURFACEDESC2 *)(dds + 4))->dwReserved >> 16);

        frames.push_back(f);
        
        
        if (!compressionFlagSetup)
        {
            const U8 * dds = reinterpret_cast<U8 *>(f->getStream(RESOURCE_STREAM_DEFAULT));
            const DDSURFACEDESC2 & desc = *reinterpret_cast<const DDSURFACEDESC2 *>(dds + 4);
            compressed = (desc.ddpfPixelFormat.dwFlags & DDPF_FOURCC);
            compressionFlagSetup = true;
        }
    }
    
    // Loading texture
    TextureManager tm;
    TRY(texture = tm->create_dynamic(width, height, &lowlevel_texture, compressed));
    // Creating vertex buffer
    buffer = SequenceManagerSingleton()->create(width, height);
}

//! Updating
bool LargeAnimatedSequence::onAddingToQueue( float dt )
{
    // Checking if parent's update function has altered frame number
    bool result = AnimatedSequenceBase::onAddingToQueue(dt);
    if (mCurrentFrameIndex != loaded_frame)
    {
        loaded_frame = mCurrentFrameIndex;
        fill_texture();
    }
    return result;
}

// Filling texture with current frame
void LargeAnimatedSequence::fill_texture()
{
    D3DLOCKED_RECT lr;
    lowlevel_texture->load();
        
    LPDIRECT3DTEXTURE9 tex = lowlevel_texture->texture;
    
    Direct3D d3d;
    static DWORD lock_flag =
        (d3d->device_caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) ? D3DLOCK_DISCARD : 0;
    
    check_directx(tex->LockRect(0, &lr, NULL, lock_flag));
    ResourceRef & f = frames[mCurrentFrameIndex];
    BYTE * ptr = reinterpret_cast<BYTE *>(f->getStream(RESOURCE_STREAM_DEFAULT)) + 128;
    
    int h = texture->compressed ?
                height >> 2 :
                height;
    u32 rowSize = static_cast<u32>(width << 2);
    for (int y = 0; y < h; ++y)
        memcpy((BYTE *)lr.pBits + y * lr.Pitch, ptr + y * rowSize, rowSize);
    check_directx(tex->UnlockRect(0));
}

// Sequence rendering
void LargeAnimatedSequence::doRender()
{
    Direct3D d3d;
    d3d->device->SetTransform(D3DTS_WORLD, &mTransformationMatrix);

    if (lowlevel_texture->load()) // load() returns true if texture was really loaded during call
        fill_texture();
    TRY(texture->bind());
    TRY(buffer->draw());
}
