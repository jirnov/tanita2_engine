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

//! ��������� ��� �������� ������ � ������� ������� �� ResourceId.
/**
 ��������� ��� �������� � �������� ������ ���������, �������
 ������������� ������� (���� ResourceId).
 �������� ������� T - ��� �������� ��������. �� ������ ����
 ����������� RefCounted � ����� ������� ���������:
     bool isEqualTo( const T & ) �
     bool isLessThen( const T & ),
 ��������� ���������� ��� �������� ������ ���� (��� ���
 packable, ��� ��� non-packable).
 
 �������� ������� Multithreaded ���������, ����� �� �����������
 ���������� ��� ������� � ����������.
 
 �������� � ���������� �������� � 256 + 1 ������� (���� vector).
 �� 256-�� ������� ���������� ������������ �������� c packable-
 ���������, ������������� �� ����. � 257-� ����� non-packable
 ��������.
 */
template<class T, bool Multithreaded=false>
class resource_container
{
public:
    // ��� ��������� ��������.
    typedef utility::reference<T> value_type;
    // ��� ��������� �� ����������.
    class const_iterator
    {
    public:
        // ������ � ��������.
        T & operator *() const;
        T * operator ->() const;
        
        // ��������� ����������
        bool operator == ( const_iterator & ) const;
        inline bool operator != ( const_iterator & i ) const
            { return !operator ==(i); }
        
        
    private:
        // ��� ������ ��������� � ��������� �� ����
        typedef std::vector<value_type> DataList;
        typedef typename DataList::iterator DataListIter;
        
        // ������ ������
        U32 mIndex;
        // ������ �� ������.
        mutable DataList & mList;
        // �������� ������ ������.
        DataListIter mListIter;
        // ��������� �� �������� �� ����� ����������
        bool mIsEnd;
        
        // �����������.
        const_iterator( DataList list[], U32 index, DataListIter iter, bool isEnd );
        
        friend class resource_container;
    };
    
    
    /* �������� ����������. ��� ���������� ������������ ��� ������
       ��������� ����: "SoundManager has released 56 unused files." */
    inline resource_container( const char * name )
        : mContainerName(name)
        {}
    
    // �������� ����������.
    inline ~resource_container() { clear(); }
    
    
    // ������� ����������.
    void clear();
    
    
    // ���������� ��������, ����������� �� ����� ����������.
    const_iterator end() const;
    
    // ����� ������� �������� � ������� ���� ������ ���������������.
    const_iterator lower_bound( const class ResourceId & what ) const;
    
    // ������� �������� � ���������.
    template<class RefType>
    value_type insert( const_iterator & where, RefType element );
    
    
    // �������� ��������� � ������� ����������� ������.
    /*
     ��� ������� �������� � ������� ����������� ������ ��������
     �-��� parent.callback (���� ��� ������), � ����� ������� ���.
     */
    void release_unused();
    
    // �������� �������� �-��� ������ ContainerOwner ��� ������� �������� ����������.
    template<class ContainerOwner>
    void for_each( ContainerOwner * parent,
                   void (ContainerOwner::*callback)( T & element ) );
    
    // �������� �������� �-��� ������-�������� ��� ������� �������� ����������.
    void for_each( void (T::*callback)() );
    void for_each_second( void (T::*callback)() );
    
    
private:
    // ����������� ���������.
    resource_container( const resource_container & );
    void operator =( const resource_container & );
    
    
    // ������ ������� ��������� (256 ������� ��� packable, ��������������
    // �� ������� 4 ����� ���� �������� � ����� � 1 ������ ��� non-packable).
    enum { DATA_LIST_COUNT = 256 + 1 };
    typedef typename const_iterator::DataList DataList;
    mutable DataList mDataList[DATA_LIST_COUNT];
    
    // ��� ��� ����������� ������
    typedef typename base::mpl::if_<Multithreaded, 
        base::sync::CriticalSection,
        base::sync::DummyCriticalSection>::type CriticalSection;
    // ����� ��� ������� ����������.
    typedef typename CriticalSection::lock_type Lock;
    // ����������� ������ ��� ���������� ������� ����� ��������.
    mutable CriticalSection mCriticalSection;
    
    // ��� ���������� (��� �������).
    const char * mContainerName;
};

} // base namespace

#include "base/container/resourceContainerDetail.h"

#endif // __BASE_CONTAINER_RESOURCECONTAINER_H__
