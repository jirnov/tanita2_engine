/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������������� ����.
 */
#pragma once
#ifndef _FLAMENCO_CORE_ATOMICTYPES_H_
#define _FLAMENCO_CORE_ATOMICTYPES_H_

#include <intrin.h>

// TODO: ���������� ��� ������ (��. atomic_f32). �������� - ������������ ��������������.

namespace flamenco
{

// ������ ����������������� ���� �������� 32 ����.
// ����������� ��������� �.�. ��� �� ������������ ��� �������������� ��������.
template <typename T32>
class atomic : noncopyable
{
public:
    FLAMENCO_STATIC_ASSERT(sizeof(T32) == sizeof(s32));
    
    atomic( T32 value )
        // ����� ����� �� ������������ � ������� �� ������� ������ :)
        : mValue(*reinterpret_cast<s32 *>(&value))
        {}
    // ���������� �������� ����������.
    inline T32 operator ()() const
    {
        s32 value = _InterlockedCompareExchange(&mValue, 0, 0);
        return *reinterpret_cast<T32 *>(&value);
    }
    // ������������� ����� ��������, ���������� ������.
    inline T32 set( T32 value )
    {
        u32 oldValue = static_cast<u32>(_InterlockedExchange(&mValue, *reinterpret_cast<s32 *>(&value)));
        return *reinterpret_cast<T32 *>(&oldValue);
    }
    
private:
    // Mutable, �.�. ��������� �������� ������� ����� ������������.
    mutable __declspec(align(32)) volatile s32 mValue;
};

// ���������������� ��� � ��������� ������������, ��������� ������
// ���������� ������ AllowedToSet.
template <typename T32, class AllowedToSet>
class atomic_readonly : public atomic<T32>
{
    atomic_readonly( T32 value )
        : atomic<T32>(value)
        {}
    
    using atomic<T32>::set;
    friend AllowedToSet;
};

// ������������� atomic ��� bool, �.�. ��� ������ ������ 32.
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
