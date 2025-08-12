#pragma once
#include "resourceManager/resourceManager.h"

class DirectoryId
{
public:
    inline DirectoryId( U32 first, ResourceType type )
        : mFirst(first), mType(type), mNonPackable(false)
        {}
    inline DirectoryId( const char * path, ResourceType type )
        : mFirst(config::res::RESOURCEID_INVALID_HASH),
          mType(type), mPath(path), mNonPackable(true)
        {}
    
    inline ResourceId toResourceId() const
        { if (mNonPackable) return ResourceId(mPath.c_str(), mType);
          return ResourceId(mFirst, mType); }
          
    inline ResourceId toResourceId( U32 second ) const
        { ASSERT(!mNonPackable);
          return ResourceId(mFirst, second, mType); }

private:
    U32 mFirst;
    ResourceType mType;
    const_string mPath;
    bool mNonPackable;
};
