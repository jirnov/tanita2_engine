#pragma once
#include "graphics/sequenceManager/sequenceManager.h"
#include "DirectoryId_.h"
#include "TextureManager_.h"

// Namespace for game object type declarations
namespace ingame
{

//! One-frame (static) sequence
class StaticSequence: public graphics::SequenceBase
{
public:
    //! Constructor
    inline StaticSequence() {}
    //! Creating static sequence for given texture
    StaticSequence( const DirectoryId & texture_path, bool compressed );
    
    virtual ~StaticSequence() {}

protected:
    //! Rendering
    virtual void doRender();
    
    // Obtaining texture
    virtual TextureRef get_texture() { return texture; }

    // Reference to texture
    TextureRef texture;
    // Reference to vertex buffer
    VertexBufferRef vbuffer;
};

}