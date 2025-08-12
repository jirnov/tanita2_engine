/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "base/baseDeclarations.h"
#include "resourceManager/resourceManager.h"

// Release all resources at once.
template<class T>
void ResourceContainer<T>::releaseAll()
{
    for (U32 i = 0; i < 257; ++i)
    {
        DataList & dataList = mDataList[i];
        for (DataListIter d = dataList.begin(); dataList.end() != d; ++d)
        {
            (*d)->doRelease();
        }
        dataList.clear();
    }
}

// Returns iterator pointing to first equal or greater element
template<class T>
typename ResourceContainer<T>::DataListIter
    ResourceContainer<T>::lowerBound( const ResourceId & path, U32 * index, bool * isEnd )
{
    if (path.isNonPackable())
    {
        *index = 256;
        DataList & dataList = mDataList[256];
        DataListIter result = dataList.begin();
        while (dataList.end() != result)
        {
            if ((*result)->isEqualTo(path))
                break;
            ++result;
        }
        *isEnd = (dataList.end() == result);
        return result;
    }
    
    // Finding lower bound
    *index = (((path.getFirstLevelHash() & 0xF) << 4) | (path.getSecondLevelHash() & 0xF));
    DataList & dataList = mDataList[*index];
    
    S32 count = static_cast<S32>(dataList.size() - 1);
    DataListIter first = dataList.begin();
    
    while (count > 0)
    {
        S32 count2 = count >> 1;
        DataListIter mid = first + count2;
        
        if (*(*mid) < path)
        {
            first = ++mid;
            count -= count2 + 1;
        }
        else
            count = count2;
    }
    *isEnd = (dataList.end() == first);
    return first;
}

// Release data elements with zero references.
template<class T>
template<class ContainerOwner>
void ResourceContainer<T>::releaseUnused( ContainerOwner & parent,
                                          void (ContainerOwner::*callback)( T & element ) )
{
#ifdef _DEBUG
    U32 releasedCount = 0;
#endif
    
    for (U32 i = 0; i < 257; ++i)
    {
        DataList & dataList = mDataList[i];
        
        // We'll create another vector of elements to avoid
        // array shuffling with lots of erases
        DataList newDataList(dataList.size());
        newDataList.resize(0);
        
        for (DataListIter d = dataList.begin(); dataList.end() != d; ++d)
            if (!d->unique())
                newDataList.push_back(*d);
            else
            {
                // Calling user pre-release function
                if (NULL != callback)
                    (parent.*callback)(*(*d));
                
                // Releasing file memory
                (*d)->doRelease();
                
#ifdef _DEBUG
                releasedCount++;
#endif
            }
            mDataList[i] = newDataList;
    }

#ifdef _DEBUG
    logger()->debug() << mContainerName << " has released "
                      << releasedCount << " unused resources";
#endif
}

// Apply callback function to each element
template<class T>
template<class ContainerOwner>
void ResourceContainer<T>::forEach( ContainerOwner & parent,
                                    void (ContainerOwner::*callback)( T & element ) )
{
    for (U32 i = 0; i < 257; ++i)
    {
        DataList & dataList = mDataList[i];
        for (DataListIter d = dataList.begin(); dataList.end() != d; ++d)
            (parent.*callback)(*(*d));
    }
}

// Apply callback function to each element
template<class T>
void ResourceContainer<T>::forEach( void (T::*callback)() )
{
    for (U32 i = 0; i < 257; ++i)
    {
        DataList & dataList = mDataList[i];
        for (DataListIter d = dataList.begin(); dataList.end() != d; ++d)
            ((*(*d)).*callback)();
    }
}


// Explicit instantiations of templates
#define RM resourceManager::ResourceManager
template class ResourceContainer<RM::ResourceInstance>;
template void ResourceContainer<RM::ResourceInstance>::
    releaseUnused<RM>( RM &, void (RM::*)( RM::ResourceInstance & ) );
#undef RM
