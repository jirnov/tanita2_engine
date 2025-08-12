/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "base/assert.h"
#include "base/sync/criticalSection.h"


using namespace base::sync;

// Инициализация секции.
CriticalSection::CriticalSection()
{
    InitializeCriticalSection(&mCriticalSection);
}

// Удаление секции.
CriticalSection::~CriticalSection()
{
    DeleteCriticalSection(&mCriticalSection);
}

// Вход в секцию.
Lock CriticalSection::doEnter()
{
    return Lock(&mCriticalSection);
}


// Конструктор по умолчанию (для наследования).
Lock::Lock()
    : mCriticalSectionPtr(NULL), mOwn(false)
{
}

// Вход в критическую секцию.
Lock::Lock( CRITICAL_SECTION * section )
    : mCriticalSectionPtr(section), mOwn(true)
{
    if (NULL != mCriticalSectionPtr)
        EnterCriticalSection(mCriticalSectionPtr);
}

// Перехватывает право на секцию (без входа в нее) у другого объекта.
Lock::Lock( Lock & lock )
    : mCriticalSectionPtr(lock.mCriticalSectionPtr),
      mOwn(true)
{
    lock.mOwn = false;
    lock.mCriticalSectionPtr = NULL;
}

// Выход из секции, если она не принадлежит кому-то другому.
Lock::~Lock()
{
    if (mOwn && NULL != mCriticalSectionPtr)
        LeaveCriticalSection(mCriticalSectionPtr);
}
