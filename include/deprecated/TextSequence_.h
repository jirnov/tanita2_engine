#pragma once
#include "graphics/sequenceManager/sequenceManager.h"

// Namespace for game object type declarations
namespace ingame
{

//! Text rendering sequence
class TextSequence: public graphics::SequenceBase
{
public:
    //! Constructor
    inline TextSequence() : color(0) {}
    //! Constructor
    inline TextSequence( D3DCOLOR color ) : color(color) {}
    
    //! Text string
    std::string text;
    
    virtual ~TextSequence() {}
    
    //! Color
    D3DCOLOR color;
    
protected:
    //! Rendering
    virtual void doRender();
};

}