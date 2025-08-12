#pragma once
#include "AnimatedSequenceBase_.h"
#include "DirectoryId_.h"
#include "TextureManager_.h"

// Namespace for game object type declarations
namespace ingame
{

//! Animated sequence for large animations
class LargeAnimatedSequence: public AnimatedSequenceBase
{
public:
    //! Creating sequence for given path and collection of frame indices
    LargeAnimatedSequence( const DirectoryId & path, std::vector<int> & frame_indices, bool compressed );

    virtual ~LargeAnimatedSequence() {}

protected:
    //! Rendering
    virtual void doRender();
    //! Updating
    virtual bool onAddingToQueue( float dt );
    
    // Obtaining texture
    virtual TextureRef get_texture() { return texture; }

    // Frames
    std::vector<ResourceRef> frames;
    // Dynamic texture
    TextureRef      texture;
    // vertex buffer
    VertexBufferRef buffer;
    // Texture width and height
    int width, height;
    
    // Index of frame loaded to texture
    int loaded_frame;
    
    // Pointer to Direct3D texture
    Direct3DInstance::TextureManagerInstance::DynamicTextureInstance * lowlevel_texture;
    
    // Fill texture with current frame image
    void fill_texture();
};

}
