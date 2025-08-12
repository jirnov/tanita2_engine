/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef __BASE_SYNC_INTERLOCKEDTYPES_H__
#define __BASE_SYNC_INTERLOCKEDTYPES_H__

#include "base/types.h"

// Классы для переменных с гарантированной 
// межпоточной синхронизацией.

namespace base {
namespace sync
{

// Класс для булевой переменной
class interlocked_bool
{
public:
    // Инциализация.
    inline interlocked_bool( bool value )
        : mValue(value ? TRUE : FALSE)
        {}
    
    // Возвращает значение переменной.
    operator bool();
    // Присваивание.
    void operator =( bool value );
    
    // Присваивает новое значение, возвращает предыдущее.
    bool set( bool value );
    
private:
    // Сама переменная.
    __declspec(align(32)) volatile S32 mValue;
    
    // Копирование запрещено.
    interlocked_bool( const interlocked_bool & );
};

// Класс для короткого целого числа со знаком
class interlocked_S32
{
public:
    // Инциализация.
    inline interlocked_S32( S32 value )
        : mValue(value)
        {}
    // Возвращает значение переменной.
    operator S32();
    // Присваивание.
    void operator =( S32 value );
    
    // Присваивает новое значение, возвращает предыдущее.
    S32 set( S32 value );
    
    // Проверяет на равенство и присваивает значение. Возвращает начальное значение.
    S32 compare_set( S32 comp, S32 value );
    
private:
    // Сама переменная.
    __declspec(align(32)) volatile S32 mValue;
    
    // Копирование запрещено.
    interlocked_S32( const interlocked_S32 & );
};

}} // base::sync namespace

#endif // __BASE_SYNC_INTERLOCKEDTYPES_H__
