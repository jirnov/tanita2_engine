/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef _RESOURCE_CLASSES_H_
#define _RESOURCE_CLASSES_H_

#include "base/sync/criticalSection.h"


namespace resourceManager
{

// Simple resource
class SimpleResourceInstance : public ResourceManager::ResourceInstance
{
public:
    // Returns pointer to data
    virtual void * getStream( ResourceStreamType stream )
        { return (stream != RESOURCE_STREAM_DEFAULT) ? NULL : (void *)mData; }
    
    // Streaming read
    virtual bool readDataBlock()
        { throw Exception("Partial read not supported"); }
    
    // Rewind
    virtual void doRewind() {}
    
    // Buffer size retrieval
    virtual U32 getDataSize() { return mDataSize; }
    
    // Sound header obtaining
    virtual const void * getHeader() { return NULL; }
    
    
protected:
    // Constructor
    explicit inline SimpleResourceInstance( const ResourceId & path, const U8 * data,
                                            U32 dataSize, bool shouldDelete )
        : ResourceManager::ResourceInstance(path),
          mData(data), mDataSize(dataSize), mShouldDeleteData(shouldDelete)
        {}
    
    // Resource memory deallocation.
    virtual void doRelease()
        { if (mData && mShouldDeleteData)
              delete [] mData; }
    
    // Update resource data with another resource of same type
    virtual void updateResource( const ResourceInstance & /* resource */ )
        { throw Exception("not implemented"); }
    
    
    // Data pointer
    const U8 * mData;
    // Data size
    U32 mDataSize;
    // Should data be deleted at end (or it will be deleted by someone else)
    bool mShouldDeleteData;
    
    // Friends
    friend class DirectResourceLoader;
    friend class PackResourceLoader;
};

// DDS resource
class DDSResourceInstance : public ResourceManager::ResourceInstance
{
public:
    // Returns pointer to texture data
    virtual void * getStream( ResourceStreamType stream )
        { return (stream != RESOURCE_STREAM_DEFAULT) ? NULL : 
                     (void *)mTextureData; }
    
    // Streaming read
    virtual bool readDataBlock()
        { throw Exception("Partial read not supported"); }
    
    // Rewind
    virtual void doRewind() {}
    
    // Buffer size retrieval
    virtual U32 getDataSize() { return mTextureDataSize; }
    
    // Header obtaining
    virtual const void * getHeader()
         { return reinterpret_cast<void *>(&mDDSHeader); }
    
    
protected:
    // Constructors
    inline DDSResourceInstance( const ResourceId & path )
        : ResourceManager::ResourceInstance(path)
        {}
    explicit DDSResourceInstance( const ResourceId & path, const U8 * data,
                                  U32 dataSize, bool shouldDelete );
    
    // Resource memory deallocation.
    virtual void doRelease()
        { if (mTextureFileData && mShouldDeleteData)
              delete [] mTextureFileData; }
    
    // Update resource data with another resource of same type
    virtual void updateResource( const ResourceInstance & /* resource */ )
        { throw Exception("not implemented"); }
    
    
    // Texture file data pointer
    const U8 * mTextureFileData;
    
    // Texture data pointer (points to native data without header)
    const U8 * mTextureData;
    // Texture data size
    U32 mTextureDataSize;
    // Should data be deleted at end (or it will be deleted by someone else)
    bool mShouldDeleteData;
    
    // Texture header
    ResourceHeaderDDS mDDSHeader;
    
    
    // Friends
    friend class DirectResourceLoader;
    friend class PackResourceLoader;
};

// PNG resource
class PNGResourceInstance : public DDSResourceInstance
{
protected:
    // Constructor
    explicit PNGResourceInstance( const ResourceId & path, const U8 * data,
                                  U32 dataSize, bool shouldDelete );
                                  
    // Friends
    friend class DirectResourceLoader;
};

// Ogg resource
class OGGResourceInstance : public ResourceManager::ResourceInstance
{
public:
    // Returns pointer to texture data
    virtual void * getStream( ResourceStreamType stream );
    
    // Streaming read
    virtual bool readDataBlock();
    
    // Rewind
    virtual void doRewind();
    
    // Buffer size retrieval
    virtual U32 getDataSize() { return 0; }
    
    // Header obtaining
    virtual const void * getHeader() { return reinterpret_cast<void *>(&mHeader); }
    
    
    base::sync::CriticalSection cs;
    
protected:
    // Constructors
    explicit OGGResourceInstance( const ResourceId & path );
    
    // Resource memory deallocation.
    virtual void doRelease();
    
    // Update resource data with another resource of same type
    virtual void updateResource( const ResourceInstance & /* resource */ )
        { throw Exception("not implemented"); }
    
    bool readData();
    
    
    // File handle
    HANDLE file_handle;
    bool eof;
    
    // Ogg variables
    ogg_sync_state container_sync_state;
    ogg_page       container_page;
    ogg_packet     container_packet;
    
    // Theora variables
    ogg_stream_state video_stream_state;
    // Vorbis variables
    ogg_stream_state audio_stream_state;
    
    // OGG header
    ResourceHeaderOGG mHeader;
    
    
    
    // Friends
    friend class DirectResourceLoader;
};

}; // End of resourceManager namespace


#endif // _RESOURCE_CLASSES_H_
