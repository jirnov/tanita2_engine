#pragma once

#include "graphics/sequenceManager/sequenceManager.h"
#include "TextureManager_.h"

//! Effects manager
class Direct3DInstance::EffectsManagerInstance
{
public:
    //! Constructor
    EffectsManagerInstance();
    //! Destructor
    ~EffectsManagerInstance();
    
    //! Reference to effect
    typedef LPD3DXEFFECT EffectRef;
    
    //! Load effect
    EffectRef load( INT resource );
    
    //! Release resources before device reset
    void release_resources();
    
    //! Restore resources after device reset
    void restore_resources();
    
protected:
    // Effects pool
    LPD3DXEFFECTPOOL pool;
    
    // List of loaded effects
    std::vector<std::pair<std::string, EffectRef> > effects;
    typedef std::vector<std::pair<std::string, EffectRef> >::iterator EffectsIter;
};

//! Reference to effect
typedef Direct3DInstance::EffectsManagerInstance::EffectRef EffectRef;

/*
namespace ingame
{

//! Sequence to begin rendering to texture
class BeginRenderTargetSequence : public graphics::SequenceBase
{
public:
    //! Constructor
    BeginRenderTargetSequence();
    
    virtual ~BeginRenderTargetSequence() {}
    
protected:
    //! Rendering
    virtual void doRender();

    // Reference to render target texture
    TextureRef texture;
    
    // Texture size
    D3DXVECTOR4 size;
    
    // Friend
    friend class GameObject;
    friend class EndRenderTargetSequence;
};

//! Sequence to end rendering to texture
class EndRenderTargetSequence : public graphics::SequenceBase
{
public:
    //! Constructor
    inline EndRenderTargetSequence() : effect(0), param_texture(0) {}
    //! Create from BeginRenderTargetSequence sequence
    EndRenderTargetSequence( const graphics::SequenceRef & s, EffectRef effect, D3DXHANDLE param_texture );

    virtual ~EndRenderTargetSequence() {}
    
protected:
    //! Rendering
    virtual void doRender();

    // Reference to render target texture
    TextureRef texture;
    // Reference to vertex buffer
    VertexBufferRef vbuffer;
    // Effect
    EffectRef effect;
    // Effect parameter for texture
    D3DXHANDLE param_texture;
};

} // End of ingame namespace
*/