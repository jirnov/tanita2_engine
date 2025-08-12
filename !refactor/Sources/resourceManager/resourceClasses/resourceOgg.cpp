/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "base/baseDeclarations.h"
#include "resourceManager/resourceManager.h"
#include "resourceManager/resourceClasses/resourceClasses.h"

using namespace resourceManager;

extern int gLoadVideoAudioState;

// Constructor
OGGResourceInstance::OGGResourceInstance( const ResourceId & path )
    : ResourceInstance(path), eof(false),
      file_handle(NULL)
{
    doRewind();
}

// Rewind
void OGGResourceInstance::doRewind()
{
    doRelease();
    eof = false;
    
    // Initializing ogg states
    ASSERT_OGG(ogg_sync_init(&container_sync_state));
    mHeader.hasVideoStream = mHeader.hasAudioStream = false;
    th_info_init(&mHeader.videoInfo);
    th_comment_init(&mHeader.videoComment);
    mHeader.ts = 0;
    mHeader.td = 0;
    vorbis_info_init(&mHeader.audioInfo);
    vorbis_comment_init(&mHeader.audioComment);
    
    ZeroMemory(&video_stream_state, sizeof(video_stream_state));
    ZeroMemory(&audio_stream_state, sizeof(audio_stream_state));
    
    // Opening video file
    file_handle = CreateFileA(mResourceId.getFilePath(), GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (INVALID_HANDLE_VALUE == file_handle)
        throw FileNotExistException(mResourceId.getFilePath());
    
    // Пропускаем метку, если она есть
    DWORD bytes_read;
    U8 mark;
    check_win32(ReadFile(file_handle, &mark, 1, &bytes_read, NULL));
    if (mark != 'x')
        SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
    
    
    // Looking for logical stream header
    bool start_header_stream = true;
    int video_header_packets = 0,  
        audio_header_packets = 0;

    do
    {
        // Reading data from stream
        if (!readData())
            // Unexpected end-of-file
            throw Exception("Unexpected EOF while reading ogg file");
        
        // Processing pages
        while (ogg_sync_pageout(&container_sync_state, &container_page) > 0)
        {
            if (!ogg_page_bos(&container_page))
            {
                // Not a header page, moving page to logical stream
                ogg_stream_pagein(&video_stream_state, &container_page);
                ogg_stream_pagein(&audio_stream_state, &container_page);
                start_header_stream = false;
                break;
            }
            else // This is a header page
            {
                // Temporary stream state
                ogg_stream_state tmp_stream_state_a, tmp_stream_state_v;

                // Initializing with serial number of page
                ASSERT_OGG(ogg_stream_init(&tmp_stream_state_a, ogg_page_serialno(&container_page)))
                ASSERT_OGG(ogg_stream_init(&tmp_stream_state_v, ogg_page_serialno(&container_page)))

                // Adding page to temporary stream
                ASSERT_OGG(ogg_stream_pagein(&tmp_stream_state_a, &container_page));
                ASSERT_OGG(ogg_stream_pagein(&tmp_stream_state_v, &container_page));

                // Decoding stream data to packet
                ogg_packet container_packet_a, container_packet_v;
                ASSERT_OGG_EXT(ogg_stream_packetout(&tmp_stream_state_a, &container_packet_a), -1);
                ASSERT_OGG_EXT(ogg_stream_packetout(&tmp_stream_state_v, &container_packet_v), -1);
                
                // Number of theora header packets read
                if (0 == video_header_packets)
                {
                    // Decoding theora header
                    if (th_decode_headerin(&mHeader.videoInfo, &mHeader.videoComment,
                                           &mHeader.ts, &container_packet_v) < 0)
                    {
                        // Not a theora header. Clearing temporary stream.
                        ogg_stream_clear(&tmp_stream_state_v);
                    }
                    else
                    {
                        // Initializing video stream from temporary stream
                        CopyMemory(&video_stream_state, &tmp_stream_state_v, sizeof(tmp_stream_state_v));
                        video_header_packets++;
                        continue;
                    }
                }
                // Number of vorbis header packets read
                if (0 == audio_header_packets)
                {
                    // Decoding vorbis header
                    if (vorbis_synthesis_headerin(&mHeader.audioInfo, &mHeader.audioComment, &container_packet_a) < 0)
                    {
                        // Not a vorbis header. Clearing temporary stream.
                        ogg_stream_clear(&tmp_stream_state_a);
                    }
                    else
                    {
                        // Initializing audio stream from temporary stream
                        CopyMemory(&audio_stream_state, &tmp_stream_state_a, sizeof(tmp_stream_state_a));
                        audio_header_packets++;
                    }
                }
            }
        }
    }
    while (start_header_stream);

    mHeader.type = RESOURCE_TYPE_OGG;
    mHeader.hasVideoStream = (gLoadVideoAudioState == 1) && (video_header_packets > 0);
    mHeader.hasAudioStream = (gLoadVideoAudioState != 1) && (audio_header_packets > 0);
    
    ASSERT(video_header_packets > 0 || audio_header_packets > 0);
    
    // Reading rest of header packets
    while ((mHeader.hasVideoStream && video_header_packets < 3) || 
           (mHeader.hasAudioStream && audio_header_packets < 3))
    {
        if (mHeader.hasVideoStream)
        {
            // Looking for theora headers
            int result;
            while ((result = ogg_stream_packetout(&video_stream_state, &container_packet)) > 0)
            {
                // Packet successfully extracted
                th_decode_headerin(&mHeader.videoInfo, &mHeader.videoComment,
                                              &mHeader.ts, &container_packet);
                if (++video_header_packets >= 3)
                    break;
            }
            if (result < 0)
                throw Exception("ogg_stream_packetout call failed");
        }
        if (mHeader.hasAudioStream)
        {
            int result;
            // Looking for vorbis headers 
            while ((result = ogg_stream_packetout(&audio_stream_state, &container_packet)) > 0)
            {
                // Packet successfully extracted
                ASSERT_OGG(vorbis_synthesis_headerin(&mHeader.audioInfo, &mHeader.audioComment, &container_packet));
                if (++audio_header_packets >= 3)
                    break;
            }
            if (result < 0)
                throw Exception("ogg_stream_packetout call failed");
        }
        
        // Reading data from file
        if (ogg_sync_pageout(&container_sync_state, &container_page) > 0)
        {
            // Reading page from buffer
            if (mHeader.hasVideoStream)
                ogg_stream_pagein(&video_stream_state, &container_page);
            if (mHeader.hasAudioStream)
                ogg_stream_pagein(&audio_stream_state, &container_page);
        }
        else
            // Reading more data from file
            if (!readData())
                // Unexpected end-of-file
                throw Exception("Unexpected EOF while reading from ogg file");
    }
    if (mHeader.hasVideoStream)
    {
        mHeader.td=th_decode_alloc(&mHeader.videoInfo,mHeader.ts);
        int pp_level=1;
        th_decode_ctl(mHeader.td,TH_DECCTL_SET_PPLEVEL,&pp_level,sizeof(pp_level));
        th_setup_free(mHeader.ts);
    }
}

// Resource memory deallocation.
void OGGResourceInstance::doRelease()
{
    if (!file_handle)
        return;
    CloseHandle(file_handle);
    
    // Clearing ogg, vorbis and theora structures
    ogg_stream_clear(&audio_stream_state);
    ogg_stream_clear(&video_stream_state);
    ogg_sync_clear(&container_sync_state);
    
    th_comment_clear(&mHeader.videoComment);
    th_info_clear(&mHeader.videoInfo);
    
    vorbis_comment_clear(&mHeader.audioComment);
    vorbis_info_clear(&mHeader.audioInfo);
}

// Returns pointer to texture data
void * OGGResourceInstance::getStream( ResourceStreamType stream )
{
    switch (stream)
    {
    case RESOURCE_STREAM_AUDIO:
        return mHeader.hasAudioStream ? reinterpret_cast<void *>(&audio_stream_state) : NULL;
    case RESOURCE_STREAM_VIDEO:
        return mHeader.hasVideoStream ? reinterpret_cast<void *>(&video_stream_state) : NULL;
    }
    return NULL;
}

bool OGGResourceInstance::readData()
{
    if (eof)
        return false;
    
    const int offset = 512 << 10;
    char * buffer = ogg_sync_buffer(&container_sync_state, offset);
    
    // Reading data from file
    DWORD bytes_read;
    check_win32(ReadFile(file_handle, buffer, offset, &bytes_read, NULL));
    ogg_sync_wrote(&container_sync_state, bytes_read);
    
    if (0 == bytes_read)
        eof = true;
    return true;
}

// Streaming read
bool OGGResourceInstance::readDataBlock()
{
    bool result = readData();
    
    // Decoding data to logical stream
    while (ogg_sync_pageout(&container_sync_state, &container_page) > 0)
    {
        if (mHeader.hasVideoStream)
            ogg_stream_pagein(&video_stream_state, &container_page);
        if (mHeader.hasAudioStream)
            ogg_stream_pagein(&audio_stream_state, &container_page);
    }
    return result;
}
