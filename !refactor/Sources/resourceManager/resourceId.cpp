/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "base/baseDeclarations.h"
#include "resourceManager/resourceManager.h"


// Buffer for file path building
char gFilePathBuffer[MAX_PATH];


// Initialization from file path string
ResourceId::ResourceId( const char * path, ResourceType type )
    : mResourceType(type), mIsNonPackable(true),
      mFirstLevelHash(config::res::RESOURCEID_INVALID_HASH),
      mSecondLevelHash(config::res::RESOURCEID_INVALID_HASH),
      mFilePathBuffer(NULL), mFilePathBufferSize(0)
{
    ASSERT(type != RESOURCE_TYPE_NONE);
    
    mFilePathBufferSize = strlen(path) + 1;
    mFilePathBuffer = new char[mFilePathBufferSize];
    strncpy(mFilePathBuffer, path, mFilePathBufferSize);
}

// Constructs path from explicitly specified hashes
void ResourceId::setFromHashes( U32 firstLevel, U32 secondLevel, ResourceType type )
{
    ASSERT(RESOURCE_TYPE_NONE != type &&
           ENGINE_RESOURCEID_INVALID_HASH != firstLevel);
    
    mResourceType = type;
    mIsNonPackable = false;
    
    mFirstLevelHash = firstLevel;
    mSecondLevelHash = secondLevel;
    
    if (NULL != mFilePathBuffer)
        delete [] mFilePathBuffer;
    mFilePathBuffer = NULL;
    mFilePathBufferSize = 0;
}

// Constructs id from another id
void ResourceId::setFromResourceId( const ResourceId & id )
{
    mResourceType = id.mResourceType;
    mIsNonPackable = id.mIsNonPackable;
    
    mFirstLevelHash  = id.mFirstLevelHash;
    mSecondLevelHash = id.mSecondLevelHash;
    
    if (mIsNonPackable)
    {
        ASSERT(NULL != id.mFilePathBuffer);
        
        U32 newSize = strlen(id.mFilePathBuffer) + 1;
        if (newSize > mFilePathBufferSize)
        {
            mFilePathBufferSize = newSize;
            if (mFilePathBuffer)
                delete [] mFilePathBuffer;
            mFilePathBuffer = new char[mFilePathBufferSize];
        }
        strncpy(mFilePathBuffer, id.mFilePathBuffer, mFilePathBufferSize);
    }
    else
    {
        if (NULL != mFilePathBuffer)
            delete [] mFilePathBuffer;
        mFilePathBuffer = NULL;
        mFilePathBufferSize = 0;
    }
}

// Returns file path string
const char * ResourceId::getFilePath() const
{
    if (mIsNonPackable)
    {
        ASSERT(mFilePathBuffer);
        return mFilePathBuffer;
    }
    ASSERT(ENGINE_RESOURCEID_INVALID_HASH != mFirstLevelHash);
    
    // Determining extension
    char * ext = NULL;
    switch (mResourceType)
    {
#define CASE(type, extension) case RESOURCE_TYPE_##type: ext = extension; break
    CASE(DDS,           "dds");
    CASE(PNG,           "png");
    CASE(WAV,           "wav");
    CASE(RGN,           "rgn");
    CASE(PTH,           "pth");
    CASE(OGG,           "ogg");
#undef CASE
    default:
        throw Exception("Invalid file type");
    }
    
    if (config::res::RESOURCEID_INVALID_HASH == mSecondLevelHash)
        sprintf(gFilePathBuffer, "art\\%x.%s", mFirstLevelHash, ext);
    else
        sprintf(gFilePathBuffer, "art\\%x\\%d.%s", mFirstLevelHash, mSecondLevelHash, ext);
    
    return gFilePathBuffer;
}

// Packable resources comparison
bool operator <( const ResourceId & a, const ResourceId & id )
{
    ASSERT(!a.mIsNonPackable && !id.mIsNonPackable);
    
    // Comparing first-level hashes
    if (a.mFirstLevelHash < id.mFirstLevelHash)
        return true;
    if (a.mFirstLevelHash != id.mFirstLevelHash)
        return false;
    
    // Comparing second-level hashes
    if (a.mSecondLevelHash < id.mSecondLevelHash)
        return true;
    if (a.mSecondLevelHash != id.mSecondLevelHash)
        return false;
    
    // Comparing resource types
    return a.mResourceType < id.mResourceType;
}
