/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef _BASE_TEMPLATES_H_
#define _BASE_TEMPLATES_H_

#include <vector>
#include "base/types.h"
#include "exceptions_.h"
#include <tanita2/utility/refcountable.h>

//! Контейнер для хранения данных с быстрым поиском по ResourceId.
/**
 Параметр шаблона - тип хранимых значений. Он должен быть наследником
 от RefCounted и иметь операторы сравнения ==, < и isEqualTo.
 
 Элементы в контейнере хранятся в 256 + 1 списках (vector). По 256
 спискам равномерно распределены элементы с нетривиальными хешами,
 в 257-м лежат non-packable элементы, у которых хеши тривиальны.
 */
template<class T>
class ResourceContainer
{
public:
    //! Тип для списка элементов
    typedef std::vector<utility::reference<T> > DataList;
    //! Тип для итератора по списку элементов.
    typedef typename DataList::iterator DataListIter;
    
    //! Конструктор. Принимает название контейнера (для отладки).
    inline ResourceContainer( const char * name )
        : mContainerName(name)
        {}
    
    //! Деструктор.
    inline ~ResourceContainer() { releaseAll(); }
    
    //! Удаление всех ресурсов.
    void releaseAll();
    
    
    //! Поиск первого большего либо равного элемента.
    /**
     Возвращает итератор, указывающий на первый больший либо равный элемент
     в (сортированном) списке, либо end(), если он не найден (в этом случае
     в переменной isEnd будет присвоено значение false.
     В переменную index записывается номер списка (0-257) для последующего
     использования в функции insert().
     */
    DataListIter lowerBound( const class ResourceId & what, U32 * index, bool * isEnd );
    
    //! Вставка элемента в контейнер.
    /**
     Добавляет новый элемент в контейнер в позицию, полученную с
     помощью вызова lowerBound() (итератор и индекс списка).
     Возвращает итератор, указывающий на добавленный элемент.
     
     @warning Между вызовами lowerBound() и insert() содержимое контейнера 
              не должен изменяться, иначе может быть нарушена сортировка.
     */
    inline DataListIter insert( DataListIter & lowerBound, U32 index, utility::reference<T> element )
        { return mDataList[index].insert(lowerBound, element); }
    
    //! Удаление элементов с нулевым количеством ссылок.
    /**
     Для каждого элемента с нулевым количеством ссылок вызывает
     ф-цию callback (если она не NULL), а затем удаляет его.
     */
    template<class ContainerOwner>
    void releaseUnused( ContainerOwner & parent,
                        void (ContainerOwner::*callback)( T & element ) = NULL );
    
    //! Вызывает заданную ф-цию класса ContainerOwner для каждого элемента контейнера.
    template<class ContainerOwner>
    void forEach( ContainerOwner & parent, 
                  void (ContainerOwner::*callback)( T & element ) );
    
    //! Вызывает заданную ф-цию класса-элемента для каждого элемента контейнера.
    void forEach( void (T::*callback)() );
    
    
private:
    // Lists of data elements organized by 4 lower bits of directory hash
    // and 4 lower bits of filename hash (((dir & 0xF) << 4) | (file & 0xF))
    DataList mDataList[257];
    
    // Container name (for debug)
    const char * mContainerName;
};

#endif // _BASE_TEMPLATES_H_
