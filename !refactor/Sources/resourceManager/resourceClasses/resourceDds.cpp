/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "base/baseDeclarations.h"
#include "resourceManager/resourceManager.h"
#include "resourceManager/resourceClasses/resourceClasses.h"
#include "graphics/graphics.h"

using namespace resourceManager;


// Constructor
DDSResourceInstance::DDSResourceInstance( const ResourceId & path, const U8 * data,
                                          U32 dataSize, bool shouldDelete )
    : ResourceInstance(path), mTextureFileData(data),
      mShouldDeleteData(shouldDelete)
{
    mTextureDataSize = dataSize;
    mTextureData = data;
    
    // Filling header
    const DDSURFACEDESC2 * desc = (const DDSURFACEDESC2 *)(data + 4);
    mDDSHeader.type = RESOURCE_TYPE_DDS;
    mDDSHeader.width = desc->dwWidth;
    mDDSHeader.height = desc->dwHeight;
    mDDSHeader.boundingBoxWidth = (desc->dwReserved & 0xFFFF);
    mDDSHeader.boundingBoxHeight = (desc->dwReserved >> 16);
    mDDSHeader.isDxtCompressed = (0 != (desc->ddpfPixelFormat.dwFlags & DDPF_FOURCC));
}

// Constructor
PNGResourceInstance::PNGResourceInstance( const ResourceId & path, const U8 * data,
                                          U32 dataSize, bool shouldDelete )
    : DDSResourceInstance(path)
{
    LPDIRECT3DTEXTURE9 tmpTexture;
    D3DXIMAGE_INFO info;
    graphics::Graphics gfx;
    check_directx(D3DXCreateTextureFromFileInMemoryEx(gfx->device, data,
        dataSize, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
        D3DFMT_DXT3,
        D3DPOOL_SYSTEMMEM, D3DX_FILTER_NONE,
        D3DX_FILTER_NONE, 0, &info, NULL, &tmpTexture));
    
    if (shouldDelete)
        delete [] data;
    
    // Saving DDS texture in memory
    LPD3DXBUFFER buf;
    check_directx(D3DXSaveTextureToFileInMemory(&buf, D3DXIFF_DDS, tmpTexture, NULL));
    DWORD size = buf->GetBufferSize();
    mTextureFileData = new U8[size];
    mShouldDeleteData = true;
    CopyMemory((U8 *)mTextureFileData, buf->GetBufferPointer(), size);
    
    // Cleanup
    buf->Release();
    tmpTexture->Release();
    
    mTextureDataSize = size;
    mTextureData = mTextureFileData;
    
    // Filling header
    DDSURFACEDESC2 * desc = (DDSURFACEDESC2 *)(mTextureData + 4);
    mDDSHeader.type = RESOURCE_TYPE_DDS;
    mDDSHeader.width = desc->dwWidth;
    mDDSHeader.height = desc->dwHeight;
    desc->dwReserved = ((info.Height << 16) | (info.Width & 0xFFFF));
    mDDSHeader.boundingBoxWidth = (desc->dwReserved & 0xFFFF);
    mDDSHeader.boundingBoxHeight = (desc->dwReserved >> 16);
    mDDSHeader.isDxtCompressed = (0 != (desc->ddpfPixelFormat.dwFlags & DDPF_FOURCC));
}
