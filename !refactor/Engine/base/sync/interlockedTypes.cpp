/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */
#include <precompiled.h>
#include "base/sync/interlockedTypes.h"

using namespace base::sync;

// _InterlockedExchange intrinsic declaration
extern "C" long _InterlockedExchange( volatile long *, long );
#pragma intrinsic (_InterlockedExchange)


// Возвращает значение переменной.
interlocked_bool::operator bool()
{
    return TRUE == _InterlockedCompareExchange(&mValue, TRUE, TRUE);
}

// Присваивание.
void interlocked_bool::operator =( bool value )
{
    _InterlockedExchange(&mValue, value);
}

// Присваивает новое значение, возвращает предыдущее.
bool interlocked_bool::set( bool value )
{
    return _InterlockedExchange(&mValue, value);
}


// Возвращает значение переменной.
interlocked_S32::operator S32()
{
    const S32 MIN_S32 = 0x80000000L;
    return _InterlockedCompareExchange(&mValue, MIN_S32, MIN_S32);
}

// Присваивание.
void interlocked_S32::operator =( S32 value )
{
    _InterlockedExchange(&mValue, value);
}

// Присваивает новое значение, возвращает предыдущее.
S32 interlocked_S32::set( S32 value )
{
    return _InterlockedExchange(&mValue, value);
}

// Проверяет на равенство и присваивает значение. Возвращает начальное значение.
S32 interlocked_S32::compare_set( S32 comp, S32 value )
{
    return _InterlockedCompareExchange(&mValue, value, comp);
}
