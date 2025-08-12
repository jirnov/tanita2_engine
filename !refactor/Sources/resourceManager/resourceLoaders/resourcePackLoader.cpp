/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "base/baseDeclarations.h"
#include "resourceManager/resourceManager.h"
#include "resourceManager/resourceLoaders/resourceLoaderClasses.h"
#include "resourceManager/resourceClasses/resourceClasses.h"
#include "lib/lzmalib/lzmalib.h"

using namespace resourceManager;


// Constructor. Loads pack file and initialized table of contents.
PackResourceLoader::PackResourceLoader()
    : mPackCount(0)
{
    // Reading table of contents
    HANDLE tocFile = CreateFileA((std::string(config::res::RESOURCE_FILE_BASENAME) + config::res::RESOURCE_TOC_EXTENSION).c_str(),
                                GENERIC_READ, 0, NULL, OPEN_EXISTING,
                                FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (INVALID_HANDLE_VALUE == tocFile)
        throw FileNotExistException("Unable to load resource table of contents.");
    
    // Reading number of packs
    U32 tmp;
    ReadFile(tocFile, &mPackCount, 4, &tmp, NULL);
    
    // Reading number of directories
    U32 directoryCount;
    ReadFile(tocFile, &directoryCount, 4, &tmp, NULL);
    mDirectories.resize(directoryCount);
    
    // Reading directories
    for (U32 dirIndex = 0; dirIndex < directoryCount; ++dirIndex)
    {
        Directory & dir = mDirectories[dirIndex];
        
        // Reading directory hash
        ReadFile(tocFile, &dir.firstLevelHash, 4, &tmp, NULL);
        // Reading file count
        U32 fileCount;
        ReadFile(tocFile, &fileCount, 4, &tmp, NULL);
        
        // Allocating memory for file list storage
        dir.files.resize(fileCount);
        // Reading files within current directory
        ReadFile(tocFile, &dir.files.front(), fileCount * sizeof(Directory::File), &tmp, NULL);
    }
    // Closing table of contents
    CloseHandle(tocFile);
    
    
    // Resizing pack list
    mPackFiles.resize(mPackCount);
    memset(&mPackFiles.front(), 0, mPackCount * sizeof(Pack));
    
    // Loading pack files
    char nameBuffer[MAX_PATH];
    for (U32 i = 0; i < (UINT)mPackCount; ++i)
    {
        Pack & pack = mPackFiles[i];
        
        // Opening file
        sprintf(nameBuffer, "%s%d%s", config::res::RESOURCE_FILE_BASENAME, i+1, 
                config::res::RESOURCE_FILE_EXTENSION);
        pack.fileHandle = CreateFileA(nameBuffer, GENERIC_READ, 0, NULL, 
                                     OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (INVALID_HANDLE_VALUE == pack.fileHandle)
            throw FileNotExistException("Unable to load data pack file.");
        
        // Checking file size
        U32 fileSize = GetFileSize(pack.fileHandle, NULL);
        check_win32(fileSize != INVALID_FILE_SIZE);
        if (0 == fileSize)
            throw Exception("Pack file has zero size.");
        /*
        // Creating mapping
        pack.mapHandle = CreateFileMapping(pack.fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
        if (INVALID_HANDLE_VALUE == pack.mapHandle)
            throw_winapi_error(GetLastError(), "Unable to create mapping for data pack file.");
        */
    }
}

void PackResourceLoader::releaseUnused()
{
    /*
    for (int i = 0; i < mPackCount; ++i)
    {
        Pack & pack = mPackFiles[i];
        if (NULL == pack.mapPtr)
            continue;
        if (pack.usage-- <= 0)
        {
            logger()->debug() << "Unloading unused pack file Resource" << i << ".pak";
            pack.usage = 0;
            check_win32(UnmapViewOfFile(pack.mapPtr));
            pack.mapPtr = NULL;
        }
    }
    */
}

// Destructor. Unloads pack file.
PackResourceLoader::~PackResourceLoader()
{
    for (std::vector<Pack>::iterator p = mPackFiles.begin();
         mPackFiles.end() != p; ++p)
    {
        // Releasing file view
        if (p->mapPtr)
            check_win32(UnmapViewOfFile(p->mapPtr));
        
        // Closing file mapping and file itself
        if (p->mapHandle)
            CloseHandle(p->mapHandle);
        if (p->fileHandle)
            CloseHandle(p->fileHandle);
    }
}

// Loads file from disk
bool PackResourceLoader::loadResource( const ResourceId & path, ResourceManager::ResourceInstance ** resource )
{
    // We are unable to load files directly from disk
    if (path.isNonPackable())
        return false;
    
    // Looking for directory
    DirectoryList::iterator directoryIter =
        lower_bound(mDirectories.begin(), mDirectories.end(), path);
    Directory & directory = *directoryIter;
    if (directory.firstLevelHash != path.getFirstLevelHash())
        return false;
    
    Directory::FileList::iterator f;
    if (path.getSecondLevelHash() != config::res::RESOURCEID_INVALID_HASH)
    {
        f = lower_bound(directory.files.begin(), directory.files.end(), path);
        if (f->secondLevelHash != path.getSecondLevelHash())
            return false;
    }
    else
        // It's not a directory but a single file
        f = directory.files.begin();
    
    ASSERT(f->resourceType == path.getType());
    
    Pack & pack = mPackFiles[f->packIndex-1];
    /*
    if (NULL == pack.mapPtr)
    {
        // Creating file view for entire file
        pack.mapPtr = MapViewOfFile(pack.mapHandle, FILE_MAP_READ, 0, 0, 0);
        if (NULL == pack.mapPtr)
            throw_winapi_error(GetLastError(), "Unable to create mapping for data pack file.");
    }
    pack.usage = 12;//120;
    */
    // File contents and size
    U8 * fileContents;
    U32 fileSize;
    bool borrowed;
    // Getting pointer to file data
    //U8 * filePtr = (U8 *)pack.mapPtr + f->offset;
    
    // Decompressing file
    if (f->uncompressedSize != f->compressedSize)
    {
        // Allocating memory for decompressed file
        fileContents = new U8[f->uncompressedSize];
        borrowed = false;
        
        // Decompressing file
        U8 * filePtr = new U8[f->compressedSize];
        SetFilePointer(pack.fileHandle, f->offset, NULL, 0);
        DWORD sizeRead;
        ReadFile(pack.fileHandle, filePtr, f->compressedSize, &sizeRead, NULL);
        fileSize = lzma_uncompress(fileContents, f->uncompressedSize,
                                   filePtr, f->compressedSize);
        delete [] filePtr;
        if (0 == fileSize)
            throw Exception(boost::str(boost::format("Decompression of file \"%s\" failed") %
                                       path.getFilePath()));
    }
    else
    {
        // File is not compressed, memory allocation is not needed
        fileContents = new U8[f->uncompressedSize];
        SetFilePointer(pack.fileHandle, f->offset, NULL, 0);
        DWORD sizeRead;
        ReadFile(pack.fileHandle, fileContents, f->uncompressedSize, &sizeRead, NULL);
        fileSize = f->uncompressedSize;
        borrowed = false;
    }
    
    // Returning loaded file
    // Creating resource
    switch (path.getType())
    {
    case RESOURCE_TYPE_DDS:
        *resource = new DDSResourceInstance(path, fileContents, fileSize, !borrowed);
        break;
    
    case RESOURCE_TYPE_RGN:
    case RESOURCE_TYPE_PTH:
    case RESOURCE_TYPE_WAV:
        *resource = new SimpleResourceInstance(path, fileContents, fileSize, !borrowed);
        break;

    default:
        throw Exception("Files of this type are not supported.");
    }
    return true;
}
