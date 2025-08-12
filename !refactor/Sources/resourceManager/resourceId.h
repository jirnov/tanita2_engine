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

//! Сравнение двух идентификаторов.
bool operator <( const class ResourceId & a, const class ResourceId & id );

//! Идентификатор ресурса.
/**
 Ресурс может быть задан как строкой с полным именем
 файла, так и набором чисел. Файлы, заданные строкой,
 называются non-packable и хранятся отдельно от обычных,
 т.к. имеют одинаковые кеши.
 */
class ResourceId
{
public:
    /**
     \group rid_init Создание и удаление.
     @{
     */
    
    //! Конструктор по умолчанию
    inline ResourceId()
        : mResourceType(RESOURCE_TYPE_NONE), mIsNonPackable(true),
          mFirstLevelHash(config::res::RESOURCEID_INVALID_HASH),
          mSecondLevelHash(config::res::RESOURCEID_INVALID_HASH),
          mFilePathBuffer(NULL), mFilePathBufferSize(0)
        {}
    
    //! Инициализация по строке с именем файла.
    explicit ResourceId( const char * path, ResourceType type );
    
    //! Инициализация по числовым идентификаторам.
    explicit inline ResourceId( U32 firstLevel, ResourceType type )
        : mFilePathBuffer(NULL), mFilePathBufferSize(0)
        { setFromHashes(firstLevel, config::res::RESOURCEID_INVALID_HASH, type); }
    //! Инициализация по числовым идентификаторам.
    explicit inline ResourceId( U32 firstLevel, U32 secondLevel, ResourceType type )
        : mFilePathBuffer(NULL), mFilePathBufferSize(0)
        { setFromHashes(firstLevel, secondLevel, type); }
    
    //! Конструктор копирования.
    inline ResourceId( const ResourceId & f )
        : mFilePathBuffer(NULL), mFilePathBufferSize(0)
        { setFromResourceId(f); }
    
    //! Инициализация по числовым идентификаторам.
    void setFromHashes( U32 firstLevel, U32 secondLevel, ResourceType type );
    //! Инициализация по другому идентификатору ресурса.
    void setFromResourceId( const ResourceId & id );
    
    //! Деструктор.
    inline ~ResourceId()
        { if (mFilePathBuffer)
              delete [] mFilePathBuffer; }
    
    /**
     @}
     \group rid_access Доступ к свойствам ресурса.
     @{
     */
    
    //! Проверка на то, что файл является non-packable.
    inline bool isNonPackable() const { return mIsNonPackable; }
    
    //! Получение хеша первого уровня.
    inline U32 getFirstLevelHash() const { return mFirstLevelHash; }
    //! Получение хеша второго уровня.
    inline U32 getSecondLevelHash()  const { return mSecondLevelHash; }
    
    //! Получение типа файла
    inline ResourceType getType() const { return mResourceType; }
    
    //! Получение строки с полным именем файла.
    const char * getFilePath() const;
    
    /**
     @}
     \group rid_comp Операции сравнения.
     @{
     */
    
    //! Проверка на равенство двух идентификаторов.
    inline bool operator ==( const ResourceId & id ) const
        { _ASSERT(!mIsNonPackable && !id.mIsNonPackable);
          return mFirstLevelHash  == id.mFirstLevelHash &&
                 mSecondLevelHash == id.mSecondLevelHash &&
                 mResourceType    == id.mResourceType; }
    
    bool isLessThen( const ResourceId & id ) const { return *this < id; }
    
    //! Проверка на равенство идентификаторов по полным путям файла.
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
