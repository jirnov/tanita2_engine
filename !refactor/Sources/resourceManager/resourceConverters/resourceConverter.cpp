/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "base/baseDeclarations.h"
#include "resourceManager/resourceManager.h"
#include "resourceManager/resourceConverters/resourceConverterClasses.h"
#include "graphics/graphics.h"

using namespace resourceManager;


// Convert PNG file to DDS
bool DDSConverter::convert( const ResourceId & path )
{
    if (path.getType() != RESOURCE_TYPE_DDS)
        return false;
    
    std::stringstream filename;
    filename << "art/" << std::setbase(16) << path.getFirstLevelHash() << ".a8r8g8b8";
    bool useCompression = true;
    if (FILE * f = fopen(filename.str().c_str(), "rt"))
    {
        useCompression = false;
        fclose(f);
    }
    
    // Constructing .png file name
    std::string tmpPathStr = path.getFilePath();
    std::string pngPath = tmpPathStr.substr(0, tmpPathStr.length() - 3) + "png";
    
    
    // Loading texture
    LPDIRECT3DTEXTURE9 tmpTexture;
    D3DXIMAGE_INFO info;
    graphics::Graphics gfx;
    check_directx(D3DXCreateTextureFromFileExA(gfx->device, pngPath.c_str(),
                       D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
                       useCompression ? D3DFMT_DXT3 : D3DFMT_A8B8G8R8,
                       D3DPOOL_SYSTEMMEM, D3DX_FILTER_NONE,
                       D3DX_FILTER_NONE, 0, &info, NULL, &tmpTexture));
    
    // Saving DDS texture in memory
    LPD3DXBUFFER buf;
    check_directx(D3DXSaveTextureToFileInMemory(&buf, D3DXIFF_DDS, tmpTexture, NULL));
    DWORD size = buf->GetBufferSize();
    U8 * contents = reinterpret_cast<U8 *>(buf->GetBufferPointer());
    
    // Saving original image info in reserved fields
    ((DDSURFACEDESC2 *)(contents + 4))->dwReserved = ((info.Height << 16) | info.Width);
    
    // Saving converted texture to file
    FILE * f = fopen(path.getFilePath(), "wb");
    if (NULL == f)
        throw Exception("Error while DDS conversion");
    fwrite(contents, size, 1, f);
    fclose(f);
    
    // Cleanup
    buf->Release();
    tmpTexture->Release();
    return true;
}
