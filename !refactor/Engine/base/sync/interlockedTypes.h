/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef __BASE_SYNC_INTERLOCKEDTYPES_H__
#define __BASE_SYNC_INTERLOCKEDTYPES_H__

#include "base/types.h"

// ������ ��� ���������� � ��������������� 
// ����������� ��������������.

namespace base {
namespace sync
{

// ����� ��� ������� ����������
class interlocked_bool
{
public:
    // ������������.
    inline interlocked_bool( bool value )
        : mValue(value ? TRUE : FALSE)
        {}
    
    // ���������� �������� ����������.
    operator bool();
    // ������������.
    void operator =( bool value );
    
    // ����������� ����� ��������, ���������� ����������.
    bool set( bool value );
    
private:
    // ���� ����������.
    __declspec(align(32)) volatile S32 mValue;
    
    // ����������� ���������.
    interlocked_bool( const interlocked_bool & );
};

// ����� ��� ��������� ������ ����� �� ������
class interlocked_S32
{
public:
    // ������������.
    inline interlocked_S32( S32 value )
        : mValue(value)
        {}
    // ���������� �������� ����������.
    operator S32();
    // ������������.
    void operator =( S32 value );
    
    // ����������� ����� ��������, ���������� ����������.
    S32 set( S32 value );
    
    // ��������� �� ��������� � ����������� ��������. ���������� ��������� ��������.
    S32 compare_set( S32 comp, S32 value );
    
private:
    // ���� ����������.
    __declspec(align(32)) volatile S32 mValue;
    
    // ����������� ���������.
    interlocked_S32( const interlocked_S32 & );
};

}} // base::sync namespace

#endif // __BASE_SYNC_INTERLOCKEDTYPES_H__
