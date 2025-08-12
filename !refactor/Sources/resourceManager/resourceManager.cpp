/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "base/baseDeclarations.h"
#include "resourceManager/resourceManager.h"
#include "resourceManager/resourceLoaders/resourceLoaderClasses.h"

#include "graphics/graphics.h"
#include "graphics/sequenceManager/sequenceManager.h"
#include "config/configDatabase.h"

using namespace resourceManager;


// Initializes file loader mechanism and memory management.
ResourceManager::ResourceManager()
    : mResourceContainer("File manager"),
      mFlagOrMask(RESOURCE_PROCESS_DEFAULT),
      mFlagAndMask(~RESOURCE_PROCESS_DEFAULT),
      mUsedMemory(0), mMaxMemory(0)
{
    // Checking configuration manager for default loader
    config::ConfigManager config;
    if (config->get<bool>("use_pack_loader"))
    {
        mResourceLoader = std::auto_ptr<PackResourceLoader>(new PackResourceLoader());
        mAuxiliaryLoader = std::auto_ptr<DirectResourceLoader>(new DirectResourceLoader());
    }
    else
        mResourceLoader = std::auto_ptr<DirectResourceLoader>(new DirectResourceLoader());
    
    // Determining memory size to use
    MEMORYSTATUS mstat;
    GlobalMemoryStatus(&mstat);
    mMaxMemory = config->get<int>("free_sysmem_threshold");
    if (mMaxMemory > 0)
        mMaxMemory <<= 10; // Mb to Kb
    else
    {
        U32 total4 = ((U32)mstat.dwTotalPhys >> 12); // Total memory / 4
        const S32 MB64 = (64 << 10); // 64Mb
        mMaxMemory = total4 > MB64 ? total4 : MB64;
    }
    
    // Printing memory usage information
    logger()->info() << "Physical memory: " << ((U32)mstat.dwTotalPhys >> 20)
                     << "Mb total, " << ((U32)mstat.dwAvailPhys >> 20)
                     << "Mb available. Using " << (mMaxMemory >> 10)
                     << "Mb for file cache";
}


S32 oldMemory;

void ResourceManager::doGarbageCollection()
{
    // \todo: better way to perform collection
    SequenceManagerSingleton sm;
    sm->doGarbageCollection();
    graphics::TextureManager tm;
    tm->collect(true); // running texture collector with hint to concentrate on RAM cleaning
    SoundManager som;
    som->doGarbageCollection();
    
    // Releasing unused files
    oldMemory = mUsedMemory;
    mResourceContainer.releaseUnused<ResourceManager>(*this, &resourceManager::ResourceManager::releaseCallback);
    
    mResourceLoader->releaseUnused();
}

// Updates memory information, releases unused files and manages file preloading
void ResourceManager::doUpdate( U32 dt )
{
    // We should try to release files only once in 5 seconds
    // to prevent collection on every frame if there are no files to
    // clean but memory usage is still high.
    static U32 timer = 0;
    timer += dt;
    
    // Checking if we need to clear memory
    if (mUsedMemory < mMaxMemory || timer < 2500)
        return;
    // Resetting timer
    timer = 0;
    
    doGarbageCollection();
    
    // Printing informational message
    U32 usage = (oldMemory - mUsedMemory) >> 10;
    if (usage > 0)
        logger()->debug() << "Releasing unused files. " << usage << "Mb freed.";
    else
    {
        static bool hintPrinted = false;
        
        if (!hintPrinted)
        {
            logger()->warn() << "There is not enough RAM for file cache. Try setting "
                                "'free_sysmem_threshold' config variable to greater value (in Mb)";
            hintPrinted = true;
        }
    }
}

// Loads file by current file loader
ResourceRef ResourceManager::loadResource( const ResourceId & path, U32 processFlags )
{
    ASSERT(NULL != mResourceLoader.get());
    
    // Or-ing and And-ing hint with global masks
    processFlags = processFlags | mFlagOrMask & mFlagAndMask;
    
    // We'll need to know where to insert file instance even if we are not going
    // to cache this file (we may wish to load it from cache later).
    U32 index;
    bool isEnd;
    ResourceList::iterator lower = mResourceContainer.lowerBound(path, &index, &isEnd);
    
    
    if ((processFlags & RESOURCE_LOAD_AUDIO) || (processFlags & RESOURCE_LOAD_VIDEO))
        processFlags |= RESOURCE_PROCESS_IGNORE_CACHE;
    
    
    // Looking for file in cache
    if (!(processFlags & RESOURCE_PROCESS_IGNORE_CACHE) && !isEnd)
    {
        if (path.isNonPackable() && (*lower)->isEqualTo(path))
            return ResourceRef(*lower);
        else if (**lower == path)
            return ResourceRef(*lower);
    }
    
    // Loading file with current loader
    ResourceInstance * f;
    if (!mResourceLoader->loadResource(path, &f))
    {
        if (!mAuxiliaryLoader->loadResource(path, &f))
            throw FileNotExistException(std::string("File not found: ") + 
                                        path.getFilePath());
    }
    
    // Adding file to cache. Path may not be changed by loader.
    mUsedMemory += (f->getDataSize() >> 10);
    
    // Inserting file in cache
    if (!(processFlags & RESOURCE_PROCESS_IGNORE_CACHE) || isEnd)
        return *mResourceContainer.insert(lower, index, utility::reference<ResourceInstance>(f));
    
    // Replacing file in cache with new one
    //(*lower)->updateResource(*f);
    return ResourceRef(f);
}
