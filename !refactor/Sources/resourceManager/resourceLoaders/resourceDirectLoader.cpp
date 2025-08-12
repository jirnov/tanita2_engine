/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "base/baseDeclarations.h"
#include "resourceManager/resourceManager.h"
#include "resourceManager/resourceLoaders/resourceLoaderClasses.h"
#include "resourceManager/resourceConverters/resourceConverterClasses.h"
#include "resourceManager/resourceClasses/resourceClasses.h"

using namespace resourceManager;


// Function for opening file for DirectFileLoader
static inline HANDLE openFile( const ResourceId & path )
{
    // GENERIC_READ access is needed for GetFileSize() function
    return CreateFileA(path.getFilePath(), GENERIC_READ, FILE_SHARE_READ,
                      NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
}


// Constructor. Initializes resource filters.
DirectResourceLoader::DirectResourceLoader()
{
    mConverters.push_back(new DDSConverter());
}

// Destructor. Releases resource filters.
DirectResourceLoader::~DirectResourceLoader()
{
    // Releasing converters
    for (ConverterIter i = mConverters.begin(); mConverters.end() != i; ++i)
        if (NULL != *i)
            delete *i;
}

// Loads resource from disk
bool DirectResourceLoader::loadResource( const ResourceId & path,
                                         ResourceManager::ResourceInstance ** resource )
{
    ASSERT(NULL != resource);
    
    if (path.getType() == RESOURCE_TYPE_OGG)
    {
        *resource = new OGGResourceInstance(path);
        return true;
    }
    
    // Loading file
    HANDLE file = openFile(path);
    if (INVALID_HANDLE_VALUE == file)
    {
        for (ConverterIter i = mConverters.begin(); mConverters.end() != i; ++i)
            if ((*i)->convert(path))
            {
                file = openFile(path);
                break;
            }
        // Complaining about missing file
        if (INVALID_HANDLE_VALUE == file)
            return false;
    }
    
    // Getting file size
    U32 size = GetFileSize(file, NULL);
    check_win32(size != INVALID_FILE_SIZE);
    
    // Allocating memory for file contents
    U8 * contents = new U8[size];
    
    // Reading file contents
    DWORD size_read;
    check_win32(ReadFile(file, contents, size, &size_read, NULL));
    ASSERT(size == size_read);
    
    // Closing file
    CloseHandle(file);
    
    // Creating resource
    switch (path.getType())
    {
    case RESOURCE_TYPE_DDS:
        *resource = new DDSResourceInstance(path, contents, size, true);
        break;
    
    case RESOURCE_TYPE_PNG:
        *resource = new PNGResourceInstance(path, contents, size, true);
        break;
    
    case RESOURCE_TYPE_RGN:
    case RESOURCE_TYPE_PTH:
    case RESOURCE_TYPE_WAV:
        *resource = new SimpleResourceInstance(path, contents, size, true);
        break;
    
    default:
        throw Exception("Files of this type are not supported.");
    }
    return true;
}
