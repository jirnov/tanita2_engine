/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef __BASE_SYNC_CRITICALSECTION_H__
#define __BASE_SYNC_CRITICALSECTION_H__

/**
 ������ � ������������ ��������.
 ��� ������ � ������������ �������� �������������
 ��� ������ - CriticalSection � Lock.
 ������ ������������ ����� �������� ��� ���������-
 ��������� ����������� ����������� ������ � 
 ��������� ��������� ������ ��� ����� � ������.
 
 ������ ������������ ����� ���������� RAII, �����������
 ���� � ����������� ������ ��� �������� � �������������
 ����� �� ��� ��� ��������.
 */

namespace base {
namespace sync
{

//
// ������ ��� ������ � ������������ ��������.
//

/**
 ����� ��� ������������� ����������� ������ � ��������
 �������� ����� � ��� ���� Lock.
 */
class CriticalSection
{
public:
    // ��� ������� ����������.
    typedef class Lock lock_type;
    
    
    // �������� ����������� ������.
    CriticalSection();
    // �������� ����������� ������.
    ~CriticalSection();
    
    // �������� ������� Lock ��� ����� � ����������� ������.
    lock_type doEnter();
    
    
private:
    // ����������� ������
    CRITICAL_SECTION mCriticalSection;
    
    // ����������� ����������� � �������� ������������
    CriticalSection( const CriticalSection & );
    void operator =( const CriticalSection & );
};

/**
 ����� ��� ����� � ����������� ������ � �������������� 
 ������� ��� �������� (������ RAII - resource acquisition 
 is initialization).
 */
class Lock
{
public:
    // ������������� ����� �� ������ (��� ����� � ���) � ������� �������.
    Lock( Lock & );
    // ����� �� ������, ���� ��� �� ����������� ����-�� �������.
    ~Lock();
    

protected:
    // ����������� �� ��������� (��� ����������� ������������).
    Lock();
    
    
private:
    // ���� � ����������� ������.
    explicit Lock( CRITICAL_SECTION * );
    
    
    // ���� �������� ������� (���������� ������������� ������ �� ��� � �����������).
    bool mOwn;
    // ��������� �� ����������� ������.
    CRITICAL_SECTION * mCriticalSectionPtr;
    
    
    // �������� ������������ ��������
    void operator =( const Lock & );
    
    friend Lock CriticalSection::doEnter();
};


//
// ������ ��� �������� ������ � ������������ ��������.
// (��� ������������� � ��������, ������� �������������� ����������)
//

/* �����-�������� ����������� ������. */
class DummyCriticalSection
{
public:
    // ��� ������� ����������.
    typedef class DummyLock lock_type;
    
    // �������� �������� ����������. ���������� ������ DummyLock.
    lock_type doEnter();
};

/* �����-�������� ����� � ����������� ������ (���� DummyCriticalSection). */
class DummyLock {};

}} // base::sync namespace

#endif // __BASE_SYNC_CRITICALSECTION_H__
