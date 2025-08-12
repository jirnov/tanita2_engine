/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ѕотокобезопасные типы.
 */
#pragma once
#ifndef _FLAMENCO_CORE_ATOMICTYPES_H_
#define _FLAMENCO_CORE_ATOMICTYPES_H_

#include <intrin.h>

// TODO: переписать под шаблон (см. atomic_f32). ѕроблема - безопасность преобразований.

namespace flamenco
{

// Ўаблон потокобезопасного типа размером 32 бита.
//  опирование запрещено т.к. тип не предназначен дл€ арифметических операций.
template <typename T32>
class atomic : noncopyable
{
public:
    FLAMENCO_STATIC_ASSERT(sizeof(T32) == sizeof(s32));
    
    atomic( T32 value )
        // «десь можно не беспокоитьс€ о доступе из другого потока :)
        : mValue(*reinterpret_cast<s32 *>(&value))
        {}
    // ¬озвращает значение переменной.
    inline T32 operator ()() const
    {
        s32 value = _InterlockedCompareExchange(&mValue, 0, 0);
        return *reinterpret_cast<T32 *>(&value);
    }
    // ”станавливает новое значение, возвращает старое.
    inline T32 set( T32 value )
    {
        u32 oldValue = static_cast<u32>(_InterlockedExchange(&mValue, *reinterpret_cast<s32 *>(&value)));
        return *reinterpret_cast<T32 *>(&oldValue);
    }
    
private:
    // Mutable, т.к. получение значени€ сделано через присваивание.
    mutable __declspec(align(32)) volatile s32 mValue;
};

// ѕотокобезопасный тип с операцией присваивани€, доступной только
// дружескому классу AllowedToSet.
template <typename T32, class AllowedToSet>
class atomic_readonly : public atomic<T32>
{
    atomic_readonly( T32 value )
        : atomic<T32>(value)
        {}
    
    using atomic<T32>::set;
    friend AllowedToSet;
};

// —пециализаци€ atomic дл€ bool, т.к. его размер меньше 32.
template<>
class atomic<bool> : noncopyable
{
public:
    atomic( bool value )
        : mValue(value)
        {}
    inline bool operator()() const
    {
        return 0 != _InterlockedCompareExchange(&mValue, 0, 0);
    }
    inline bool set( bool value )
    {
        return 0 != _InterlockedExchange(&mValue, value);
    }
    
private:
    mutable __declspec(align(32)) volatile s32 mValue;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_ATOMICTYPES_H_
