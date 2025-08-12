/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef _RESOURCE_ID_H_
#define _RESOURCE_ID_H_

// Forward declaration
namespace resourceManager
{
    class ResourceManager;
}

//! ��������� ���� ���������������.
bool operator <( const class ResourceId & a, const class ResourceId & id );

//! ������������� �������.
/**
 ������ ����� ���� ����� ��� ������� � ������ ������
 �����, ��� � ������� �����. �����, �������� �������,
 ���������� non-packable � �������� �������� �� �������,
 �.�. ����� ���������� ����.
 */
class ResourceId
{
public:
    /**
     \group rid_init �������� � ��������.
     @{
     */
    
    //! ����������� �� ���������
    inline ResourceId()
        : mResourceType(RESOURCE_TYPE_NONE), mIsNonPackable(true),
          mFirstLevelHash(config::res::RESOURCEID_INVALID_HASH),
          mSecondLevelHash(config::res::RESOURCEID_INVALID_HASH),
          mFilePathBuffer(NULL), mFilePathBufferSize(0)
        {}
    
    //! ������������� �� ������ � ������ �����.
    explicit ResourceId( const char * path, ResourceType type );
    
    //! ������������� �� �������� ���������������.
    explicit inline ResourceId( U32 firstLevel, ResourceType type )
        : mFilePathBuffer(NULL), mFilePathBufferSize(0)
        { setFromHashes(firstLevel, config::res::RESOURCEID_INVALID_HASH, type); }
    //! ������������� �� �������� ���������������.
    explicit inline ResourceId( U32 firstLevel, U32 secondLevel, ResourceType type )
        : mFilePathBuffer(NULL), mFilePathBufferSize(0)
        { setFromHashes(firstLevel, secondLevel, type); }
    
    //! ����������� �����������.
    inline ResourceId( const ResourceId & f )
        : mFilePathBuffer(NULL), mFilePathBufferSize(0)
        { setFromResourceId(f); }
    
    //! ������������� �� �������� ���������������.
    void setFromHashes( U32 firstLevel, U32 secondLevel, ResourceType type );
    //! ������������� �� ������� �������������� �������.
    void setFromResourceId( const ResourceId & id );
    
    //! ����������.
    inline ~ResourceId()
        { if (mFilePathBuffer)
              delete [] mFilePathBuffer; }
    
    /**
     @}
     \group rid_access ������ � ��������� �������.
     @{
     */
    
    //! �������� �� ��, ��� ���� �������� non-packable.
    inline bool isNonPackable() const { return mIsNonPackable; }
    
    //! ��������� ���� ������� ������.
    inline U32 getFirstLevelHash() const { return mFirstLevelHash; }
    //! ��������� ���� ������� ������.
    inline U32 getSecondLevelHash()  const { return mSecondLevelHash; }
    
    //! ��������� ���� �����
    inline ResourceType getType() const { return mResourceType; }
    
    //! ��������� ������ � ������ ������ �����.
    const char * getFilePath() const;
    
    /**
     @}
     \group rid_comp �������� ���������.
     @{
     */
    
    //! �������� �� ��������� ���� ���������������.
    inline bool operator ==( const ResourceId & id ) const
        { _ASSERT(!mIsNonPackable && !id.mIsNonPackable);
          return mFirstLevelHash  == id.mFirstLevelHash &&
                 mSecondLevelHash == id.mSecondLevelHash &&
                 mResourceType    == id.mResourceType; }
    
    bool isLessThen( const ResourceId & id ) const { return *this < id; }
    
    //! �������� �� ��������� ��������������� �� ������ ����� �����.
    inline bool isEqualTo( const ResourceId & id ) const
        { _ASSERT(mIsNonPackable && id.mIsNonPackable);
          bool equal = strcmp(mFilePathBuffer, id.getFilePath());
          return (0 != equal) ? false : (mResourceType == id.mResourceType); }
    
    /** @} */
    
    
protected:
    // File name hashes
    U32 mFirstLevelHash,
        mSecondLevelHash;
    
    // Resource is non-packable (is accessed by path, not by hashes)
    bool mIsNonPackable;
    
    
    // File path string pointer
    char * mFilePathBuffer;
    // Length of path string buffer
    U32 mFilePathBufferSize;
    
    
    // Resource type
    ResourceType mResourceType;
    
    
private:
    // Assignment operator is prohibited
    void operator =( const ResourceId & );
    
    
    // Friends
    friend class resourceManager::ResourceManager;
    friend bool operator <( const class ResourceId & a, const class ResourceId & id );
};

#endif // _RESOURCE_ID_H_
