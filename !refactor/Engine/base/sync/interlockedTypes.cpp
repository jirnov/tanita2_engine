/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */
#include <precompiled.h>
#include "base/sync/interlockedTypes.h"

using namespace base::sync;

// _InterlockedExchange intrinsic declaration
extern "C" long _InterlockedExchange( volatile long *, long );
#pragma intrinsic (_InterlockedExchange)


// ���������� �������� ����������.
interlocked_bool::operator bool()
{
    return TRUE == _InterlockedCompareExchange(&mValue, TRUE, TRUE);
}

// ������������.
void interlocked_bool::operator =( bool value )
{
    _InterlockedExchange(&mValue, value);
}

// ����������� ����� ��������, ���������� ����������.
bool interlocked_bool::set( bool value )
{
    return _InterlockedExchange(&mValue, value);
}


// ���������� �������� ����������.
interlocked_S32::operator S32()
{
    const S32 MIN_S32 = 0x80000000L;
    return _InterlockedCompareExchange(&mValue, MIN_S32, MIN_S32);
}

// ������������.
void interlocked_S32::operator =( S32 value )
{
    _InterlockedExchange(&mValue, value);
}

// ����������� ����� ��������, ���������� ����������.
S32 interlocked_S32::set( S32 value )
{
    return _InterlockedExchange(&mValue, value);
}

// ��������� �� ��������� � ����������� ��������. ���������� ��������� ��������.
S32 interlocked_S32::compare_set( S32 comp, S32 value )
{
    return _InterlockedCompareExchange(&mValue, value, comp);
}
