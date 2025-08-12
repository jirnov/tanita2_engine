/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef _RESOURCE_LOADER_CLASSES_H_
#define _RESOURCE_LOADER_CLASSES_H_


namespace resourceManager
{

// Base class for resource loaders
class ResourceLoaderBase
{
public:
    // Destructor
    virtual ~ResourceLoaderBase() = 0 {}
    
    // Resource loading
    virtual bool loadResource( const ResourceId & path,
                               ResourceManager::ResourceInstance ** resource ) = 0;
    
    virtual void releaseUnused() {}
};

// Class for loading resources from disk
class DirectResourceLoader : public ResourceLoaderBase
{
public:
    // Constructor. Initializes resource contents filters.
    DirectResourceLoader();
    
    // Destructor. Releases resource contents filters.
    virtual ~DirectResourceLoader();
    
    
    // Loads resource from disk
    virtual bool loadResource( const ResourceId & path,
                               ResourceManager::ResourceInstance ** resource );
    
    
private:
    // Type of resource converters list and iterator
    typedef std::vector<class ResourceConverterBase *> ConverterList;
    typedef ConverterList::iterator ConverterIter;
    
    // List of resource converters
    ConverterList mConverters;
};

// Class for loading resources from resource pack.
class PackResourceLoader : public ResourceLoaderBase
{
public:
    // Constructor. Loads resource packs and table of contents.
    PackResourceLoader();
    
    // Destructor. Unloads resource packs.
    ~PackResourceLoader();
    
    
    // Loads resource from resource pack
    virtual bool loadResource( const ResourceId & path,
                               ResourceManager::ResourceInstance ** resource );
    
    void releaseUnused();
    
private:
    // Type for resource meta-information stored in table of contents
    struct ResourceHeader
    {
        // Resource header
        union
        {
            ResourceHeaderDDS headerDDS;
        };
        
        // Offset to user data
        U32 userDataOffset;
    };
    
    
    // Directory representation
    struct Directory
    {
        // File representation
        struct File
        {
            // Resource type
            U32 resourceType;
            
            // Second level hash
            U32 secondLevelHash;
            
            // Index of pack file
            U32 packIndex;
            
            // Size of uncompressed file
            U32 uncompressedSize;
            // Size of compressed file
            U32 compressedSize;
            
            // Offset to file in pack
            U32 offset;
            
            // File header
            ResourceHeader header;
        };
        
        
        // Constructor
        inline Directory()
            : firstLevelHash(0) {}
        
        
        // Directory hash
        U32 firstLevelHash;
        
        // Type for file list
        typedef std::vector<File> FileList;
        // List of files
        FileList files;
    };
    
    // Pack file representation
    struct Pack
    {
        // File handle
        HANDLE fileHandle;
        // Mapping handle
        HANDLE mapHandle;
        // Pointer to mapped data
        void * mapPtr;
        int usage;
    };
    
    
    // Resource pack files
    std::vector<Pack> mPackFiles;
    
    int mPackCount;
    
    // Type for directory list
    typedef std::vector<Directory> DirectoryList;
    // List of directories
    DirectoryList mDirectories;
    
    friend bool operator <( const PackResourceLoader::Directory & dir, const PackResourceLoader::Directory & d2 );
    friend bool operator <( const PackResourceLoader::Directory & dir, const ResourceId & path );
    friend bool operator <( const PackResourceLoader::Directory::File & f, const ResourceId & path );
    friend bool operator <( const PackResourceLoader::Directory::File & f, const PackResourceLoader::Directory::File & f2 );
    friend bool operator <( const ResourceId & path, const PackResourceLoader::Directory & dir );
    friend bool operator <( const ResourceId & path, const PackResourceLoader::Directory::File & f );
};

inline bool operator <( const PackResourceLoader::Directory & dir, const PackResourceLoader::Directory & d2 )
{
    return dir.firstLevelHash < d2.firstLevelHash;
}

// Comparison operator for lower_bound
inline bool operator <( const PackResourceLoader::Directory & dir, const ResourceId & path )
{
    return dir.firstLevelHash < path.getFirstLevelHash();
}
inline bool operator <( const ResourceId & path, const PackResourceLoader::Directory & dir )
{
    return !(dir < path);
}

// Comparison operator
inline bool operator <( const PackResourceLoader::Directory::File & f, const ResourceId & path )
{
    return f.secondLevelHash < path.getSecondLevelHash();
}
inline bool operator <( const PackResourceLoader::Directory::File & f, const PackResourceLoader::Directory::File & f2 )
{
    return f.secondLevelHash < f2.secondLevelHash;
}
// Comparison operator
inline bool operator <( const ResourceId & path, const PackResourceLoader::Directory::File & f )
{
    return !(f < path);
}

}; // End of resourceManager namespace


#endif // _RESOURCE_LOADER_CLASSES_H_
