#pragma once
#include "graphics/sequenceManager/sequenceManager.h"

// Namespace for game object type declarations
namespace ingame
{

//! Base class for animated sequence
class AnimatedSequenceBase: public graphics::SequenceBase
{
public:
    //! Constructor
    AnimatedSequenceBase( std::vector<int> & frame_indices );
    //! Destructor
    virtual ~AnimatedSequenceBase() = 0 {}
    
    // Used by effects pipeline to get current sequence texture
    //virtual TextureRef getCurrentTexture() { return TextureRef(); }
        
protected:
    //! Updating
    virtual bool onAddingToQueue( float dt );
    
    // Time left for current frame
    float time_left;
};

}