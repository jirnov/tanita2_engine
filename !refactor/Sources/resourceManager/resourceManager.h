/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include "base/templates.h"
#include <tanita2/utility/refcountable.h>

//! Типы ресурсов.
enum ResourceType
{
    //! Пустой (неправильный) ресурс
    RESOURCE_TYPE_NONE = 0,
    //! Текстура в формате DDS
    RESOURCE_TYPE_DDS,
    //! Текстура в формате PNG
    RESOURCE_TYPE_PNG,
    //! Звуковой файл в формате WAV (не поточный)
    RESOURCE_TYPE_WAV,
    //! Файл с точками региона.
    RESOURCE_TYPE_RGN,
    //! Файл с точками пути.
    RESOURCE_TYPE_PTH,
    //! Файл в формате OGG/OGM (всегда поточный)
    RESOURCE_TYPE_OGG,
};

/**
 @defgroup rm_load_flags Флаги обработки ресурса.
 Флаги, подсказывающие файловому менеджеру, как
 надо загружать и обрабатывать ресурс.
 */

//! Флаг по умолчанию.
#define RESOURCE_PROCESS_DEFAULT (U32)0

//! Загрузить ресурс минуя кеш ресурсов.
/**
 Если ресурса в кеше нет, загрузить его и добавить в кеш.
 Если файл уже есть в кеше, все равно загрузить его и заменить
 содержимое кеша новыми данными.
 
 @warning Флаг должен использоваться с осторожностью для потоковых файлов,
          иначе на середине чтения его данные могут быть заменены.
 */
#define RESOURCE_PROCESS_IGNORE_CACHE (U32)1

//! Файл должен быть загружен напрямую с диска.
/**
 Файл должен быть загружен напрямую, даже если используется другой
 механизм загрузки (напр., из ресурс-пака).
 */
#define RESOURCE_PROCESS_DIRECT_LOADING (U32)2

//! Не сохранять преобразованный в нативный формат файл на диск.
/**
 Если ресурс загружается из не нативного формата, результат
 его преобразования в нативный формат не должен быть сохранен на диск.
 */
#define RESOURCE_PROCESS_CONVERT_IN_MEMORY_ONLY (U32)8

#define RESOURCE_LOAD_AUDIO (U32)16
#define RESOURCE_LOAD_VIDEO (U32)32

/** @} */


//! Типы потоков.
enum ResourceStreamType
{
    //! Поток по умолчанию.
    RESOURCE_STREAM_DEFAULT,
    //! Поток звуковых данных.
    RESOURCE_STREAM_AUDIO = RESOURCE_STREAM_DEFAULT,
    //! Поток видео данных.
    RESOURCE_STREAM_VIDEO,
};


// ResourceId declaration
#include "resourceManager/resourceId.h"

// Resource header declarations
#include "resourceManager/resourceHeaders.h"
#include <tanita2/utility/refcountable.h>

//! Пространство имен для менеджера ресурсов.
namespace resourceManager
{

//! Менеджер ресурсов.
class ResourceManager
{
public:
    // Forward declarations
    class ResourceInstance;
    
    //! Ссылка на ресурс (используется за пределами менеджера ресурсов)
    typedef utility::reference<ResourceInstance> ResourceRef;
    
    
    //! Инициализация.
    ResourceManager();
    
    //! Деструктор.
    inline ~ResourceManager() {}
    
    
    void doGarbageCollection();
    
    //! Обновление состояния менеджера ресурсов.
    /**
     Периодически удаляет никем не используемые ресурсы.
     */
    void doUpdate( U32 dt );
    
    
    //! Загрузка ресурса.
    ResourceRef loadResource( const ResourceId & id, 
                              U32 processFlags = RESOURCE_PROCESS_DEFAULT );
    
    
    //! Установить маски флагов обработки ресурса.
    /**
     При загрузке ресурса флаги параметров обработки and-ятся и
     or-ятся с данными масками, что приводит к принудительной
     установке или сбросу флагов, напр., глобальное отключение
     ресурсного кеша.
     */
    inline void setFlagMasks( U32 orMask, U32 andMask )
        { mFlagOrMask = orMask; mFlagAndMask = andMask; }
    
    
    //! Интерфейс для работы с ресурсом.
    /**
     У каждого ресурса есть заголовок и как минимум один поток.
     Поток представляет собой указатель на родные для клиента 
     данные (их формат зависит от типа ресурса).
     */
    class ResourceInstance : public utility::refcountable
    {
    public:
        //! Получение указателя на поток данных
        virtual void * getStream( ResourceStreamType stream ) = 0;
        
        //! Чтение очередной порции файла во внутренний буфер
        /**
         Применяется главным образом для поточных файлов. Возвращает
         false если достигнут конец потока.
         */
        virtual bool readDataBlock() = 0;
        
        //! Возвращает курсор чтения на начало потока.
        virtual void doRewind() = 0;
    
        //! Получение заголовка файла
        virtual const void * getHeader() = 0;
        
        //! Получение размера данных ресурса в байтах
        /**
         Для некоторых типов ресурсов получение размера невозможно,
         для них возвращается 0.
         */
        virtual U32 getDataSize() = 0;
        
        
    public:
        //! Сравнение ресурсов по путям.
        inline bool operator ==( const ResourceId & id )
            { return mResourceId == id; }
        //! Сравнение ресурсов по путям.
        inline bool operator < ( const ResourceId & id )
            { return mResourceId < id; }
        //! Сравнение ресурсов по путям (для non-packable ресурсов).
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


//! Ссылка на ресурс (используется за пределами менеджера ресурсов)
typedef resourceManager::ResourceManager::ResourceRef ResourceRef;

#endif // _RESOURCE_MANAGER_H_
