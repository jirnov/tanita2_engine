/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef __BASE_CONTAINER_RESOURCECONTAINER_H__
#define __BASE_CONTAINER_RESOURCECONTAINER_H__

#include "base/sync/criticalSection.h"
#include "base/mpl/if_.h"
#include <tanita2/utility/refcountable.h>

namespace base
{

//! Контейнер для хранения данных с быстрым поиском по ResourceId.
/**
 Контейнер для хранения и быстрого поиска элементов, имеющих
 идентификатор ресурса (типа ResourceId).
 Параметр шаблона T - тип хранимых значений. Он должен быть
 наследником RefCounted и иметь функции сравнения:
     bool isEqualTo( const T & ) и
     bool isLessThen( const T & ),
 корректно работающие для ресурсов одного типа (или оба
 packable, или оба non-packable).
 
 Параметр шаблона Multithreaded указывает, нужны ли межпоточные
 блокировки при доступе к контейнеру.
 
 Элементы в контейнере хранятся в 256 + 1 списках (типа vector).
 По 256-ти спискам равномерно распределены элементы c packable-
 ресурсами, сортированные по хешу. В 257-м лежат non-packable
 элементы.
 */
template<class T, bool Multithreaded=false>
class resource_container
{
public:
    // Тип хранимого значения.
    typedef utility::reference<T> value_type;
    // Тип итератора по контейнеру.
    class const_iterator
    {
    public:
        // Доступ к элементу.
        T & operator *() const;
        T * operator ->() const;
        
        // Сравнение итераторов
        bool operator == ( const_iterator & ) const;
        inline bool operator != ( const_iterator & i ) const
            { return !operator ==(i); }
        
        
    private:
        // Тип списка элементов и итератора по нему
        typedef std::vector<value_type> DataList;
        typedef typename DataList::iterator DataListIter;
        
        // Индекс списка
        U32 mIndex;
        // Ссылка на список.
        mutable DataList & mList;
        // Итератор внутри списка.
        DataListIter mListIter;
        // Указывает ли итератор на конец контейнера
        bool mIsEnd;
        
        // Конструктор.
        const_iterator( DataList list[], U32 index, DataListIter iter, bool isEnd );
        
        friend class resource_container;
    };
    
    
    /* Создание контейнера. Имя контейнера используется при печати
       сообщений вида: "SoundManager has released 56 unused files." */
    inline resource_container( const char * name )
        : mContainerName(name)
        {}
    
    // Удаление контейнера.
    inline ~resource_container() { clear(); }
    
    
    // Очистка контейнера.
    void clear();
    
    
    // Возвращает итератор, указывающий на конец контейнера.
    const_iterator end() const;
    
    // Поиск первого элемента с большим либо равным идентификатором.
    const_iterator lower_bound( const class ResourceId & what ) const;
    
    // Вставка элемента в контейнер.
    template<class RefType>
    value_type insert( const_iterator & where, RefType element );
    
    
    // Удаление элементов с нулевым количеством ссылок.
    /*
     Для каждого элемента с нулевым количеством ссылок вызывает
     ф-цию parent.callback (если она задана), а затем удаляет его.
     */
    void release_unused();
    
    // Вызывает заданную ф-цию класса ContainerOwner для каждого элемента контейнера.
    template<class ContainerOwner>
    void for_each( ContainerOwner * parent,
                   void (ContainerOwner::*callback)( T & element ) );
    
    // Вызывает заданную ф-цию класса-элемента для каждого элемента контейнера.
    void for_each( void (T::*callback)() );
    void for_each_second( void (T::*callback)() );
    
    
private:
    // Копирование запрещено.
    resource_container( const resource_container & );
    void operator =( const resource_container & );
    
    
    // Списки списков элементов (256 списков для packable, распределяемых
    // по младшим 4 битам хеша каталога и файла и 1 список для non-packable).
    enum { DATA_LIST_COUNT = 256 + 1 };
    typedef typename const_iterator::DataList DataList;
    mutable DataList mDataList[DATA_LIST_COUNT];
    
    // Тип для критической секции
    typedef typename base::mpl::if_<Multithreaded, 
        base::sync::CriticalSection,
        base::sync::DummyCriticalSection>::type CriticalSection;
    // Класс для объекта блокировки.
    typedef typename CriticalSection::lock_type Lock;
    // Критическая секция для разделения доступа между потоками.
    mutable CriticalSection mCriticalSection;
    
    // Имя контейнера (для отладки).
    const char * mContainerName;
};

} // base namespace

#include "base/container/resourceContainerDetail.h"

#endif // __BASE_CONTAINER_RESOURCECONTAINER_H__
