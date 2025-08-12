/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include "base/templates.h"
#include <tanita2/utility/refcountable.h>

//! ���� ��������.
enum ResourceType
{
    //! ������ (������������) ������
    RESOURCE_TYPE_NONE = 0,
    //! �������� � ������� DDS
    RESOURCE_TYPE_DDS,
    //! �������� � ������� PNG
    RESOURCE_TYPE_PNG,
    //! �������� ���� � ������� WAV (�� ��������)
    RESOURCE_TYPE_WAV,
    //! ���� � ������� �������.
    RESOURCE_TYPE_RGN,
    //! ���� � ������� ����.
    RESOURCE_TYPE_PTH,
    //! ���� � ������� OGG/OGM (������ ��������)
    RESOURCE_TYPE_OGG,
};

/**
 @defgroup rm_load_flags ����� ��������� �������.
 �����, �������������� ��������� ���������, ���
 ���� ��������� � ������������ ������.
 */

//! ���� �� ���������.
#define RESOURCE_PROCESS_DEFAULT (U32)0

//! ��������� ������ ����� ��� ��������.
/**
 ���� ������� � ���� ���, ��������� ��� � �������� � ���.
 ���� ���� ��� ���� � ����, ��� ����� ��������� ��� � ��������
 ���������� ���� ������ �������.
 
 @warning ���� ������ �������������� � ������������� ��� ��������� ������,
          ����� �� �������� ������ ��� ������ ����� ���� ��������.
 */
#define RESOURCE_PROCESS_IGNORE_CACHE (U32)1

//! ���� ������ ���� �������� �������� � �����.
/**
 ���� ������ ���� �������� ��������, ���� ���� ������������ ������
 �������� �������� (����., �� ������-����).
 */
#define RESOURCE_PROCESS_DIRECT_LOADING (U32)2

//! �� ��������� ��������������� � �������� ������ ���� �� ����.
/**
 ���� ������ ����������� �� �� ��������� �������, ���������
 ��� �������������� � �������� ������ �� ������ ���� �������� �� ����.
 */
#define RESOURCE_PROCESS_CONVERT_IN_MEMORY_ONLY (U32)8

#define RESOURCE_LOAD_AUDIO (U32)16
#define RESOURCE_LOAD_VIDEO (U32)32

/** @} */


//! ���� �������.
enum ResourceStreamType
{
    //! ����� �� ���������.
    RESOURCE_STREAM_DEFAULT,
    //! ����� �������� ������.
    RESOURCE_STREAM_AUDIO = RESOURCE_STREAM_DEFAULT,
    //! ����� ����� ������.
    RESOURCE_STREAM_VIDEO,
};


// ResourceId declaration
#include "resourceManager/resourceId.h"

// Resource header declarations
#include "resourceManager/resourceHeaders.h"
#include <tanita2/utility/refcountable.h>

//! ������������ ���� ��� ��������� ��������.
namespace resourceManager
{

//! �������� ��������.
class ResourceManager
{
public:
    // Forward declarations
    class ResourceInstance;
    
    //! ������ �� ������ (������������ �� ��������� ��������� ��������)
    typedef utility::reference<ResourceInstance> ResourceRef;
    
    
    //! �������������.
    ResourceManager();
    
    //! ����������.
    inline ~ResourceManager() {}
    
    
    void doGarbageCollection();
    
    //! ���������� ��������� ��������� ��������.
    /**
     ������������ ������� ����� �� ������������ �������.
     */
    void doUpdate( U32 dt );
    
    
    //! �������� �������.
    ResourceRef loadResource( const ResourceId & id, 
                              U32 processFlags = RESOURCE_PROCESS_DEFAULT );
    
    
    //! ���������� ����� ������ ��������� �������.
    /**
     ��� �������� ������� ����� ���������� ��������� and-���� �
     or-���� � ������� �������, ��� �������� � ��������������
     ��������� ��� ������ ������, ����., ���������� ����������
     ���������� ����.
     */
    inline void setFlagMasks( U32 orMask, U32 andMask )
        { mFlagOrMask = orMask; mFlagAndMask = andMask; }
    
    
    //! ��������� ��� ������ � ��������.
    /**
     � ������� ������� ���� ��������� � ��� ������� ���� �����.
     ����� ������������ ����� ��������� �� ������ ��� ������� 
     ������ (�� ������ ������� �� ���� �������).
     */
    class ResourceInstance : public utility::refcountable
    {
    public:
        //! ��������� ��������� �� ����� ������
        virtual void * getStream( ResourceStreamType stream ) = 0;
        
        //! ������ ��������� ������ ����� �� ���������� �����
        /**
         ����������� ������� ������� ��� �������� ������. ����������
         false ���� ��������� ����� ������.
         */
        virtual bool readDataBlock() = 0;
        
        //! ���������� ������ ������ �� ������ ������.
        virtual void doRewind() = 0;
    
        //! ��������� ��������� �����
        virtual const void * getHeader() = 0;
        
        //! ��������� ������� ������ ������� � ������
        /**
         ��� ��������� ����� �������� ��������� ������� ����������,
         ��� ��� ������������ 0.
         */
        virtual U32 getDataSize() = 0;
        
        
    public:
        //! ��������� �������� �� �����.
        inline bool operator ==( const ResourceId & id )
            { return mResourceId == id; }
        //! ��������� �������� �� �����.
        inline bool operator < ( const ResourceId & id )
            { return mResourceId < id; }
        //! ��������� �������� �� ����� (��� non-packable ��������).
        inline bool isEqualTo( const ResourceId & id )
            { return mResourceId.isEqualTo(id); }
        inline bool isLessThen( const ResourceId & id )
            { return mResourceId.isLessThen(id); }
        
    
    private:
        // Copy constructor and assignment operator are prohibited
        ResourceInstance( const ResourceInstance & );
        void operator =( const ResourceInstance & );
        
        
    protected:
        // Constructor
        inline ResourceInstance( const ResourceId & path )
            : mResourceId(path)
            {}
        
        // Resource memory deallocation.
        virtual void doRelease() = 0;
        
        // Update resource data with another resource of same type
        virtual void updateResource( const ResourceInstance & resource ) = 0;
        
        
        // Resource identifier
        ResourceId mResourceId;
        
        
        // Friends
        friend class ResourceManager;
        template <class T> friend class ResourceContainer;
    };
private:
    // Flag masks for or-ing and and-ing with load flags.
    // Used to override some flags (for example, disable caching globally)
    U32 mFlagOrMask, mFlagAndMask;
    
    
    // Current resource loader
    boost::shared_ptr<class ResourceLoaderBase> mResourceLoader;
    // Auxiliary resource loader (may be NULL)
    boost::shared_ptr<class ResourceLoaderBase> mAuxiliaryLoader;
    
    
    // Container for fast resource search
    ResourceContainer<ResourceInstance> mResourceContainer;
    // Type definition for resource list
    typedef ResourceContainer<ResourceInstance>::DataList ResourceList;
    
    
    // Size of memory used for file cache (in Kb)
    S32 mUsedMemory;
    // Maximum size of memory to use
    S32 mMaxMemory;
    
    
    // Callback function for updating memory counter before file release
    inline void releaseCallback( ResourceInstance & resource )
        { mUsedMemory -= (resource.getDataSize() >> 10); }
    
    
    // Friend classes
    friend class ResourceLoaderBase;
    friend class DirectResourceLoader;
    friend class PackResourceLoader;
    friend class SimpleResourceInstance;
    friend class DDSResourceInstance;
    friend class OGGResourceInstance;
    template <class T> friend class ResourceContainer;
};

}; // End of resourceManager namespace


//! ������ �� ������ (������������ �� ��������� ��������� ��������)
typedef resourceManager::ResourceManager::ResourceRef ResourceRef;

#endif // _RESOURCE_MANAGER_H_
