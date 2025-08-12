#include <precompiled.h>
#include "StaticSequence_.h"

using namespace ingame;

// Constructor
StaticSequence::StaticSequence( const DirectoryId & texture_path, bool /* compressed */ )
{
    // Loading texture
    TextureManager tm;
    TRY(texture = tm->load(texture_path.toResourceId(), true));
    DDSURFACEDESC2 desc = texture->get_description();

    // Creating vertex buffer
    vbuffer = SequenceManagerSingleton()->create(desc.dwWidth, desc.dwHeight);
    
    mBoundingBox.x = float(desc.dwReserved & 0xFFFF);
    mBoundingBox.y = float(desc.dwReserved >> 16);
}

// Sequence rendering
void StaticSequence::doRender()
{
    Direct3D d3d;
    d3d->device->SetTransform(D3DTS_WORLD, &mTransformationMatrix);

    TRY(texture->bind());
    TRY(vbuffer->draw());
}
