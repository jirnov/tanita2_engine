#include <precompiled.h>
#include "VideoSequence_.h"
#include "Application_.h"

using namespace ingame;

// Creating video sequence for given path
VideoSequenceBase::VideoSequenceBase( char * path )
    : need_rewind(false), video_stream_state(NULL),
      mResourceId(path, RESOURCE_TYPE_OGG), mIsPlayingPrev(false)
{
    initialize();
}

// Unloading resources
void VideoSequenceBase::unload()
{
}

void VideoSequenceBase::doStop()
{
    mIsPlaying = false;
    sound->doStop();
}

int gLoadVideoAudioState;

// Initialize playback
void VideoSequenceBase::initialize()
{
    // Misc initializations
    time_elapsed = 0;
    need_texture_fill = false;
    
    gLoadVideoAudioState = 1;
    
    ResourceManagerSingleton fm;
    mResource = fm->loadResource(mResourceId, RESOURCE_LOAD_VIDEO);
    mResource->doRewind();
    videobuf_granulepos = -1;
    
    // Initializing video stream
    header = (ResourceHeaderOGG *)(mResource->getHeader());
    if (header->type != RESOURCE_TYPE_OGG || !header->hasVideoStream)
        throw Exception("OGG file is not a video file");
    video_stream_state = (ogg_stream_state *)(mResource->getStream(RESOURCE_STREAM_VIDEO));

    // Creating audio sequence
    gLoadVideoAudioState = 2;
    
    SoundManager sm;
    sound = sm->loadSound(mResourceId);
    sound->setVolume(100);
    sound->setGroupId(2);
    sound->setIsNonExpirable(false);
    sound->setIsNonPositionable(true);
    
    gLoadVideoAudioState = 0;
}


// Updating
bool VideoSequenceBase::onAddingToQueue( float dt )
{
    mIsLoopOver = false;
    if (mIsPlaying)
    {
        // Rewinding if needed
        if (need_rewind)
        {
            initialize();
            need_rewind = false;
        }
        // Playing sound
        sound->addToQueue();
            
        if (mIsPlaying && mIsPlayingPrev != mIsPlaying)
            sound->doPlay();
        
        need_texture_fill = false;
        bool filled = (dt <= 0);
        while (!filled)
        {
            for (;;)
            {
                float frame_time = float(th_granule_time(header->td, videobuf_granulepos));
                if (frame_time >= time_elapsed)
                {
                    filled = true;
                    break;
                }
                need_texture_fill = true;
            
                // Reading data from buffer
                if (ogg_stream_packetout(video_stream_state, &container_packet) > 0)
                {
                    th_decode_packetin(header->td, &container_packet, &videobuf_granulepos);
                }
                else
                {
                    if (!mResource->readDataBlock())
                    {
                        mIsPlaying = false;
                        mIsLoopOver = true;
                        sound->doStop();
                        time_elapsed = 0;
                    }
                }
            }
            
            if (filled)
                break;
        }
        time_elapsed = sound->getPlayPosition() / 1000.0f;
        
        // Decoding frame
        if (need_texture_fill)
        {
            ASSERT_OGG(th_decode_ycbcr_out(header->td, video_yuv_buffer));
        }
    }
    else
    {
        time_elapsed = 0;
        need_rewind = true;
        sound->doStop();
    }
    mIsPlayingPrev = mIsPlaying;
    return SequenceBase::onAddingToQueue(dt);
}

//! Constructor
HWVideoSequence::HWVideoSequence( char * path )
    : VideoSequenceBase(path), target_texture(NULL), yuv_texture(NULL)
{
    // Creating textures
    TextureManager tm;
    
    UINT wp2 = 1, hp2 = 1;
    while (wp2 < header->videoInfo.frame_width)  wp2 <<= 1;
    while (hp2 < header->videoInfo.frame_height) hp2 <<= 1;
    TRY(yuv_texture_ref = tm->create_surface_texture(wp2, hp2, D3DFMT_YUY2, &yuv_texture));
    TRY(texture = tm->create_surface_texture(wp2, hp2, D3DFMT_X8R8G8B8, &target_texture, true));
 
    // Filling yuv surface with black
    D3DLOCKED_RECT lr;
    yuv_texture->load();
    check_directx(yuv_texture->surface->LockRect(&lr, NULL, D3DLOCK_DISCARD));
    ZeroMemory(lr.pBits, lr.Pitch * hp2);
    check_directx(yuv_texture->surface->UnlockRect());
    
    // Creating vertex buffer
    buffer = SequenceManagerSingleton()->create(wp2, hp2);
}

// Sequence rendering
void HWVideoSequence::doRender()
{
    Direct3D d3d;
    d3d->device->SetTransform(D3DTS_WORLD, &mTransformationMatrix);

    if (target_texture->load()) // load() returns true if texture was really loaded during call
        fill_texture();
    TRY(texture->bind());
    TRY(buffer->draw());
}

// Updating
bool HWVideoSequence::onAddingToQueue( float dt )
{
    bool result = VideoSequenceBase::onAddingToQueue(dt);
    if (need_texture_fill)
        fill_texture();
    return result;
}

// Fill texture with data
void HWVideoSequence::fill_texture()
{
    // Creating textures (if needed)
    yuv_texture->load();
    target_texture->load();
    
    if (need_texture_fill)
    {
        // Filling YUV surface with data
        D3DLOCKED_RECT lr;
        check_directx(yuv_texture->surface->LockRect(&lr, NULL, D3DLOCK_DISCARD));

        const int width  = header->videoInfo.frame_width,
                height = header->videoInfo.frame_height,
                y_stride = video_yuv_buffer[0].stride,
                uv_stride = video_yuv_buffer[1].stride;
        register BYTE * frame_byte = (BYTE *)lr.pBits;
        const BYTE * y = video_yuv_buffer[0].data,
                * u = video_yuv_buffer[1].data,
                * v = video_yuv_buffer[2].data;
        
        for (int i = 0; i < height; ++i)
        {
            const int y_shift = y_stride * i;
            const int uv_shift = uv_stride * (i >> 1);

            for (int j = 0; j < width >> 1; ++j)
            {
                *frame_byte++ = *(BYTE *)(y + y_shift + (j << 1));
                *frame_byte++ = *(BYTE *)(u + uv_shift + j);
                *frame_byte++ = *(BYTE *)(y + y_shift + (j << 1) + 1);
                *frame_byte++ = *(BYTE *)(v + uv_shift + j);
            }
            frame_byte += (yuv_texture->width - width) << 1;
        }
        check_directx(yuv_texture->surface->UnlockRect());
    }
    // Blitting to target texture
    Direct3D d3d;
    check_directx(d3d->device->StretchRect(yuv_texture->surface, NULL, 
                                            target_texture->surface, NULL, D3DTEXF_POINT));
}

// Creating video sequence for given path
CPUConversionVideoSequence::CPUConversionVideoSequence( char * path )
    : VideoSequenceBase(path), frame_data(NULL),
      lowlevel_texture(NULL), frame_data_stride(0)
{
    // Creating texture
    TextureManager tm;
    UINT wp2 = 1, hp2 = 1;
    while (wp2 < header->videoInfo.frame_width)  wp2 <<= 1;
    while (hp2 < header->videoInfo.frame_height) hp2 <<= 1;
    frame_height_pow2 = hp2;
    TRY(texture = tm->create_dynamic(wp2, hp2, &lowlevel_texture, false));
    
    // Creating vertex buffer
    buffer = SequenceManagerSingleton()->create(wp2, hp2);
    
    // Creating frame buffer
    frame_data_stride = wp2 * 4;
    frame_data = new BYTE[hp2 * frame_data_stride];
    ZeroMemory(frame_data, hp2 * frame_data_stride);
}

// Sequence rendering
void CPUConversionVideoSequence::doRender()
{
    Direct3D d3d;
    d3d->device->SetTransform(D3DTS_WORLD, &mTransformationMatrix);

    if (lowlevel_texture->load()) // load() returns true if texture was really loaded during call
        fill_texture();
    
    TRY(texture->bind());
    TRY(buffer->draw());
}

// Updating
bool CPUConversionVideoSequence::onAddingToQueue( float dt )
{
    bool result = VideoSequenceBase::onAddingToQueue(dt);
    if (need_texture_fill)
        fill_texture();
    return result;
}

// Unloading resources
void CPUConversionVideoSequence::unload()
{
    // Freeing frame buffer
    if (frame_data)
    {
        delete[] frame_data;
        frame_data = NULL;
    }
    VideoSequenceBase::unload();
}

// Clamping to byte
static __forceinline BYTE clamp( int value )
{
    return (BYTE)(value > 255 ? 255 : (value < 0 ? 0 : value));
}

// Fill texture with data
void CPUConversionVideoSequence::fill_texture()
{
    ASSERT(frame_data);
    if (!need_texture_fill)
        return;
    
    // YUV to RGB conversion table (for CPU caching)
    int b0_[256];
    int b1_[256];
    int b2_[256];
    int b3_[256];
    
    for(signed int i=0; i < 256; ++i)
        b0_[i] = (113443 * (i - 128) + 32768) >> 16;
    for(signed int i=0; i < 256; ++i)
        b1_[i] = (45744 * (i - 128) + 32768) >> 16;
    for(signed int i=0; i < 256; ++i)
        b2_[i] = (22020 * (i - 128) + 32768) >> 16;
    for(signed int i=0; i < 256; ++i)
        b3_[i] = (113508 * (i - 128) + 32768) >> 16;

    const int width  = header->videoInfo.frame_width,
              height = header->videoInfo.frame_height,
              y_stride = video_yuv_buffer[0].stride,
              uv_stride = video_yuv_buffer[1].stride;
    register BYTE * frame_byte = frame_data;
    const BYTE * y = video_yuv_buffer[0].data,
               * u = video_yuv_buffer[1].data,
               * v = video_yuv_buffer[2].data;
    
    for (int i = 0; i < height; i += 2)
    {
        const int y_shift = y_stride * i;
        const int uv_shift = uv_stride * (i >> 1);

        for (int j = 0; j < width; ++j)
        {
            const int tmp_j = j >> 1;

            const int nY = (int)(*(unsigned char*)(y + y_shift + j));
            const unsigned char nU = *(u + uv_shift + tmp_j);
            const unsigned char nV = *(v + uv_shift + tmp_j);

            const int r = nY + b0_[nV];
            const int g = nY - b1_[nV] - b2_[nU];
            const int b = nY + b3_[nU];

            frame_byte[2] = clamp(r);
            frame_byte[1] = clamp(g);
            frame_byte[0] = clamp(b);
            frame_byte[3] = 0xff;
            frame_byte += 4;
        }
        frame_byte += (frame_data_stride << 1) - (width << 2);
        memcpy(frame_byte - frame_data_stride, frame_byte - (frame_data_stride << 1), (width << 2));
    }
    
    // Writing uncompressed frame to texture
    D3DLOCKED_RECT lr;
    lowlevel_texture->load();

    LPDIRECT3DTEXTURE9 tex = lowlevel_texture->texture;
    Direct3D d3d;
    static DWORD lock_flag =
        (d3d->device_caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) ? D3DLOCK_DISCARD : 0;

    check_directx(tex->LockRect(0, &lr, NULL, lock_flag));
    memcpy((BYTE *)lr.pBits, frame_data, frame_data_stride * frame_height_pow2);
    check_directx(tex->UnlockRect(0));
}
