#pragma once
#include "EffectsManager_.h"
#include "sound/sound.h"

// Namespace for game object type declarations
namespace ingame
{

//! Video sequence
class VideoSequenceBase: public graphics::SequenceBase
{
public:
    //! Creating sequence for given path
    VideoSequenceBase( char * path );
    
    //! Destructor
    virtual ~VideoSequenceBase() = 0 {}

protected:
    //! Initialize playback
    void initialize();

    //! Unload sequence data
    virtual void unload();

    //! Rendering
    virtual void doRender() = 0;
    
    void doStop();

    //! Updating
    virtual bool onAddingToQueue( float dt );
    
    ogg_stream_state * video_stream_state;
    ogg_packet     container_packet;
    //theora_state     video_state;
    th_ycbcr_buffer       video_yuv_buffer;
    ogg_int64_t         videobuf_granulepos;
    
    ResourceHeaderOGG * header;
    
    
    // Time from video start
    float time_elapsed;
    
    // Texture update needed
    bool need_texture_fill;
    
    // Sound sequence
    SoundRef sound;
    
    bool mIsPlayingPrev;
    
    // Video file path
    ResourceId mResourceId;
    ResourceRef mResource;
    
    // Rewind flag
    bool need_rewind;
};

//! Video sequence with YUV conversion by CPU
class CPUConversionVideoSequence: public VideoSequenceBase
{
public:
    //! Constructor
    CPUConversionVideoSequence( char * path );
    
    virtual ~CPUConversionVideoSequence() {}

protected:
    //! Unload sequence data
    virtual void unload();

    //! Rendering
    virtual void doRender();

    //! Updating
    virtual bool onAddingToQueue( float dt );
    
    //! Fill texture with frame data
    void fill_texture();
    
    // Frame data pointer
    BYTE * frame_data;
    int frame_data_stride;
    // Texture height
    int frame_height_pow2;
    
    // Target texture
    TextureRef texture;
    // Vertex buffer
    VertexBufferRef buffer;
    // Pointer to dynamic texture
    Direct3DInstance::TextureManagerInstance::DynamicTextureInstance * lowlevel_texture;
};

//! Video sequence with hardware YUV conversion
class HWVideoSequence: public VideoSequenceBase
{
public:
    //! Constructor
    HWVideoSequence( char * path );
    
    virtual ~HWVideoSequence() {}

protected:
    //! Unload sequence data
    virtual void unload() {}

    //! Rendering
    virtual void doRender();

    //! Updating
    virtual bool onAddingToQueue( float dt );
    
    //! Fill texture with frame data
    void fill_texture();
    
    // Target texture
    TextureRef texture;
    // Vertex buffer
    VertexBufferRef buffer;
    // Pointer to target texture instance
    Direct3DInstance::TextureManagerInstance::SurfaceTextureInstance * target_texture;
    
    // YUV texture
    TextureRef yuv_texture_ref;
    Direct3DInstance::TextureManagerInstance::SurfaceTextureInstance * yuv_texture;
};

class ShaderVideoSequence: public VideoSequenceBase
{
public:
    //! Constructor
    ShaderVideoSequence( char * path );
    
    virtual ~ShaderVideoSequence() {}

protected:
    //! Unload sequence data
    virtual void unload() {}

    //! Rendering
    virtual void doRender();

    //! Updating
    virtual bool onAddingToQueue( float dt );
    
    //! Fill texture with frame data
    void fill_texture();
    
    // Target texture
    TextureRef texture;
    // Vertex buffer
    VertexBufferRef buffer;
    // Pointer to target texture instance
    Direct3DInstance::TextureManagerInstance::SurfaceTextureInstance * target_texture;
    
    EffectRef effect;
    D3DXHANDLE matrixHandle, textureHandle, textureSizeHandle, videoSizeHandle;
};

} // End of ::ingame namespace
