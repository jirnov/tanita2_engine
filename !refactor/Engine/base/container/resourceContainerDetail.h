/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef __BASE_CONTAINER_RESOURCECONTAINERDETAIL_H__
#define __BASE_CONTAINER_RESOURCECONTAINERDETAIL_H__

namespace base
{

//
// Итератор.
//

// Конструктор.
template<class T, bool MT>
resource_container<T, MT>::const_iterator::const_iterator(
    typename resource_container<T, MT>::const_iterator::DataList list[],
    U32 index,
    typename resource_container<T, MT>::const_iterator::DataListIter iter,
    bool isEnd )
    
    : mList(list[index]), mIndex(index), mListIter(iter), mIsEnd(isEnd)
{
}

// Доступ к элементу.
template<class T, bool MT>
T & resource_container<T, MT>::const_iterator::operator *() const
{
    return *(*mListIter);
}

// Доступ к элементу.
template<class T, bool MT>
T * resource_container<T, MT>::const_iterator::operator ->() const
{
    return *mListIter;
}

// Сравнение итераторов
template<class T, bool MT>
bool resource_container<T, MT>::const_iterator::operator ==(
    typename resource_container<T, MT>::const_iterator & iter ) const
{
    if (mIsEnd && iter.mIsEnd)
        return true;
    return mIndex == iter.mIndex && mListIter == iter.mListIter;
}

//
// Контейнер.
//

// Очистка контейнера.
template<class T, bool MT>
void resource_container<T, MT>::clear()
{
    Lock lock(mCriticalSection.doEnter());
    
    for (U32 i = 0; i < DATA_LIST_COUNT; ++i)
    {
        DataList & dataList = mDataList[i];
        dataList.clear();
    }
}

// Возвращает итератор, указывающий на конец контейнера.
template<class T, bool MT>
typename resource_container<T, MT>::const_iterator resource_container<T, MT>::end() const
{
    return const_iterator(mDataList, 0, mDataList[0].end(), true);
}

// Поиск первого элемента с большим либо равным идентификатором.
template<class T, bool MT>
typename resource_container<T, MT>::const_iterator
    resource_container<T, MT>::lower_bound( const class ResourceId & what ) const
{
    if (what.isNonPackable())
    {
        const U32 index = DATA_LIST_COUNT-1;
        DataList & dataList = mDataList[index];
        DataList::iterator result = dataList.begin();
        while (dataList.end() != result)
        {
            if ((*result)->isEqualTo(what))
                break;
            ++result;
        }
        if (dataList.end() == result)
            return end();
        return const_iterator(mDataList, index, result, false);
    }
    
    // Finding lower bound
    U32 index = (((what.getFirstLevelHash() & 0xF) << 4) | (what.getSecondLevelHash() & 0xF));
    DataList & dataList = mDataList[index];
    
    S32 count = dataList.size()-1;
    const_iterator::DataListIter first = dataList.begin();
   
    while (count > 0)
    {
        S32 count2 = count >> 1;
        const_iterator::DataListIter mid = first + count2;
        
        if ((*mid)->isLessThen(what))
        {
            first = ++mid;
            count -= count2 + 1;
        }
        else
            count = count2;
    }
    if (dataList.end() == first)
        return end();
    return const_iterator(mDataList, index, first, false);
}

// Вставка элемента в контейнер.
template<class T, bool MT>
template<class RefType>
typename resource_container<T, MT>::value_type
    resource_container<T, MT>::insert( 
        typename resource_container<T, MT>::const_iterator & where, 
        RefType element )
{
    Lock lock(mCriticalSection.doEnter());
    
    return *where.mList.insert(where.mListIter, element);
}

// Удаление элементов с нулевым количеством ссылок.
template<class T, bool MT>
void resource_container<T, MT>::release_unused()
{
    U32 releasedCount = 0;
    
    // Код, нуждающийся в межпоточной блокировке.
    {
        Lock lock(mCriticalSection.doEnter());
        
        for (U32 i = 0; i < DATA_LIST_COUNT; ++i)
        {
            DataList & dataList = mDataList[i];
            
            for (DataList::iterator d = dataList.begin(); dataList.end() != d;)
                if (!d->unique())
                    ++d;
                else
                {
                    // Releasing element memory
                    releasedCount++;
                    d = dataList.erase(d);
                }
        }
    }
    
    if (config::DEBUG)
        logger()->debug() << mContainerName << " has released "
                          << releasedCount << " unused resources of type "
                          << typeid(T).name();
}

// Вызывает заданную ф-цию класса ContainerOwner для каждого элемента контейнера.
template<class T, bool MT>
template<class ContainerOwner>
void resource_container<T, MT>::for_each(
    ContainerOwner * parent,
    void (ContainerOwner::*callback)( T & element ) )
{
    _ASSERT(NULL != parent);
    
    for (U32 i = 0; i < DATA_LIST_COUNT; ++i)
    {
        DataList & dataList = mDataList[i];
        for (DataList::iterator d = dataList.begin(); dataList.end() != d; ++d)
            (parent.*callback)(*(*d));
    }
}

// Вызывает заданную ф-цию класса ContainerOwner для каждого элемента контейнера.
template<class T, bool MT>
void resource_container<T, MT>::for_each( void (T::*callback)() )
{
    for (U32 i = 0; i < DATA_LIST_COUNT; ++i)
    {
        DataList & dataList = mDataList[i];
        for (DataList::iterator d = dataList.begin(); dataList.end() != d; ++d)
            ((*(*d)).*callback)();
    }
}

// Вызывает заданную ф-цию класса ContainerOwner для каждого элемента контейнера.
template<class T, bool MT>
void resource_container<T, MT>::for_each_second( void (T::*callback)() )
{
    for (U32 i = 0; i < DATA_LIST_COUNT; ++i)
    {
        {
            Lock lock(mCriticalSection.doEnter());
            DataList & dataList = mDataList[i];
            for (DataList::iterator d = dataList.begin(); dataList.end() != d; ++d)
                ((*(*d)).*callback)();
        }
    }
}


} // namespace base

#endif // __BASE_CONTAINER_RESOURCECONTAINERDETAIL_H__
