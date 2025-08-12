/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������������� ����.
 */
#pragma once
#ifndef _FLAMENCO_CORE_LOCK_H_
#define _FLAMENCO_CORE_LOCK_H_

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace flamenco
{

// ����������� ������.
class critical_section : noncopyable
{
private:
    inline void enter()
    {
        EnterCriticalSection(&mCS);
    }
    inline void leave()
    {
        LeaveCriticalSection(&mCS);
    }
    
public:
    // ��� ��� RAII-������� ��� ����������� ����������.
    typedef scoped_lock<critical_section,
                        &critical_section::enter, &critical_section::leave> lock;
    
    critical_section()
    {
        InitializeCriticalSection(&mCS);
    }
    ~critical_section()
    {
        DeleteCriticalSection(&mCS);
    }
    
private:
    CRITICAL_SECTION mCS;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_LOCK_H_
