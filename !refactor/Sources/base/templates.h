/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef _BASE_TEMPLATES_H_
#define _BASE_TEMPLATES_H_

#include <vector>
#include "base/types.h"
#include "exceptions_.h"
#include <tanita2/utility/refcountable.h>

//! ��������� ��� �������� ������ � ������� ������� �� ResourceId.
/**
 �������� ������� - ��� �������� ��������. �� ������ ���� �����������
 �� RefCounted � ����� ��������� ��������� ==, < � isEqualTo.
 
 �������� � ���������� �������� � 256 + 1 ������� (vector). �� 256
 ������� ���������� ������������ �������� � �������������� ������,
 � 257-� ����� non-packable ��������, � ������� ���� ����������.
 */
template<class T>
class ResourceContainer
{
public:
    //! ��� ��� ������ ���������
    typedef std::vector<utility::reference<T> > DataList;
    //! ��� ��� ��������� �� ������ ���������.
    typedef typename DataList::iterator DataListIter;
    
    //! �����������. ��������� �������� ���������� (��� �������).
    inline ResourceContainer( const char * name )
        : mContainerName(name)
        {}
    
    //! ����������.
    inline ~ResourceContainer() { releaseAll(); }
    
    //! �������� ���� ��������.
    void releaseAll();
    
    
    //! ����� ������� �������� ���� ������� ��������.
    /**
     ���������� ��������, ����������� �� ������ ������� ���� ������ �������
     � (�������������) ������, ���� end(), ���� �� �� ������ (� ���� ������
     � ���������� isEnd ����� ��������� �������� false.
     � ���������� index ������������ ����� ������ (0-257) ��� ������������
     ������������� � ������� insert().
     */
    DataListIter lowerBound( const class ResourceId & what, U32 * index, bool * isEnd );
    
    //! ������� �������� � ���������.
    /**
     ��������� ����� ������� � ��������� � �������, ���������� �
     ������� ������ lowerBound() (�������� � ������ ������).
     ���������� ��������, ����������� �� ����������� �������.
     
     @warning ����� �������� lowerBound() � insert() ���������� ���������� 
              �� ������ ����������, ����� ����� ���� �������� ����������.
     */
    inline DataListIter insert( DataListIter & lowerBound, U32 index, utility::reference<T> element )
        { return mDataList[index].insert(lowerBound, element); }
    
    //! �������� ��������� � ������� ����������� ������.
    /**
     ��� ������� �������� � ������� ����������� ������ ��������
     �-��� callback (���� ��� �� NULL), � ����� ������� ���.
     */
    template<class ContainerOwner>
    void releaseUnused( ContainerOwner & parent,
                        void (ContainerOwner::*callback)( T & element ) = NULL );
    
    //! �������� �������� �-��� ������ ContainerOwner ��� ������� �������� ����������.
    template<class ContainerOwner>
    void forEach( ContainerOwner & parent, 
                  void (ContainerOwner::*callback)( T & element ) );
    
    //! �������� �������� �-��� ������-�������� ��� ������� �������� ����������.
    void forEach( void (T::*callback)() );
    
    
private:
    // Lists of data elements organized by 4 lower bits of directory hash
    // and 4 lower bits of filename hash (((dir & 0xF) << 4) | (file & 0xF))
    DataList mDataList[257];
    
    // Container name (for debug)
    const char * mContainerName;
};

#endif // _BASE_TEMPLATES_H_
