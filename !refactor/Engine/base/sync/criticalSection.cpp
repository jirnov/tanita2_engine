/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "base/assert.h"
#include "base/sync/criticalSection.h"


using namespace base::sync;

// ������������� ������.
CriticalSection::CriticalSection()
{
    InitializeCriticalSection(&mCriticalSection);
}

// �������� ������.
CriticalSection::~CriticalSection()
{
    DeleteCriticalSection(&mCriticalSection);
}

// ���� � ������.
Lock CriticalSection::doEnter()
{
    return Lock(&mCriticalSection);
}


// ����������� �� ��������� (��� ������������).
Lock::Lock()
    : mCriticalSectionPtr(NULL), mOwn(false)
{
}

// ���� � ����������� ������.
Lock::Lock( CRITICAL_SECTION * section )
    : mCriticalSectionPtr(section), mOwn(true)
{
    if (NULL != mCriticalSectionPtr)
        EnterCriticalSection(mCriticalSectionPtr);
}

// ������������� ����� �� ������ (��� ����� � ���) � ������� �������.
Lock::Lock( Lock & lock )
    : mCriticalSectionPtr(lock.mCriticalSectionPtr),
      mOwn(true)
{
    lock.mOwn = false;
    lock.mCriticalSectionPtr = NULL;
}

// ����� �� ������, ���� ��� �� ����������� ����-�� �������.
Lock::~Lock()
{
    if (mOwn && NULL != mCriticalSectionPtr)
        LeaveCriticalSection(mCriticalSectionPtr);
}
