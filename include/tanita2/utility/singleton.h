/*
 ������ "������2".
 ������� ����������.
 */
#pragma once
#ifndef _TANITA2_UTILITY_SINGLETON_H_
#define _TANITA2_UTILITY_SINGLETON_H_

#include <boost/noncopyable.hpp>

namespace utility
{

//! ������ ��������� � ��������� ���������� �������.
/**
 ���������� ��������� � ��������� ���������� ������-
 ��������� T ������� (����� ����� doCreate()).
 ��������� ��� ����������� ����������� ������� ��������
 ����������� ������ �������.
 
 �������������:
 \code
 class SomeClass
 {
 public:
     void a();
     int b;
 };
 
 void testSingletonUsage()
 {
     manual_singleton<SomeClass> mySingleton;
     
     // ��������� ����� mySingleton->a() �������� �
     // ����������, �.�. ��������� ��� �� ������.
     
     mySingleton.doCreate(); // �������� ����������
     
     // ������ � ���������
     mySingleton->a();
     mySingleton->b = 0;
     
     // ���� �������� ������ ����� �� ������������,
     // �� ����� ������ �������������.
 }
 \endcode
 */
template<class T>
class manual_singleton : boost::noncopyable
{
public:
    // ����������� �� ���������.
    manual_singleton() {}
    
    //! �������� ����������.
    /**
     ������� ���������� ���� ��������� ��� ���������� ��� �����-�� �����������,
     �� ��� ������.
     */
    void doCreate()
    {
        checkBeforeCreation();
        initialize(new T());
    }
    
    //! �������� ���������� � �������������� ������������ �������.
    /**
     ������� ��������� ������ T ������� ������������ �� ����������� initializer.
     ������� ���������� ���� ��������� ��� ���������� ��� �����-�� 
     �����������, �� ��� ������.
     */
    template<class Initializer>
    void doCreate( const Initializer & initializer )
    {
        checkBeforeCreation();
        initialize(new T(initializer));
    }
    
    //! ���������� �������� � �������� ����������, ���� ������� �������.
    ~manual_singleton()
    {
        if (NULL == mInstance)
        {
            assert(0 == mRefCount);
            return;
        }
        if (mRefCount == 0xeefeeefe || mRefCount == 0xfeeefeee)
            DebugBreak();
        if (--mRefCount > 0)
            return;
        
        assert(0 == mRefCount);
        delete mInstance;
        mInstance = NULL;
    }
    
    //! ������ � ����������.
    const T * operator ->() const
    {
        assert(NULL != mInstance);
        return mInstance;
    }
    T * operator ->()
    {
        return const_cast<T *>(const_cast<const manual_singleton *>(this)->operator ->());
    }
    
    //! �������� �� �������������� ����������.
    static bool unique()
    {
        assert(mInstance);
        return (1 == mRefCount);
    }
    
    //! �������� ������������� ����������.
    static bool exists()
    {
        return NULL != mInstance;
    }
    
protected:
    // ���������� �������� ������.
    void incRefCount()
    {
        assert(mInstance);
        mRefCount++;
    }
    
private:
    // �������� ������������ ���������� ������.
    void checkBeforeCreation() const
    {
        if (NULL != mInstance)
            throw std::logic_error("Singleton instance already exists");
        if (mIsCreatedSecondTime)
            throw std::logic_error("Singleton instance created second time");
    }
    // ������������� ������.
    void initialize( T * newptr )
    {
        assert(NULL != newptr);
        mInstance = newptr;
        mIsCreatedSecondTime = true;
        mRefCount++;
    }
    
    // ������� ������.
    static s32 mRefCount;
    // ��������� �� ���������.
    static T * mInstance;
    // ���� ��� �������� ���������� ��������.
    static bool mIsCreatedSecondTime;
};

// ������� ������.
template<class T> s32 manual_singleton<T>::mRefCount;
// ��������� �� ���������.
template<class T> T * manual_singleton<T>::mInstance;
// ���� ��� �������� ���������� ��������.
template<class T> bool manual_singleton<T>::mIsCreatedSecondTime = false;


//! ������ ���������, �� ���������� ���������.
/**
 ���������� ����������, �� ��������� ���������, ����
 �� ��� �� ����������. ��� �������� ���������� ������
 �������������� manual_singleton.
 
 ������ �������������:
 \code
 class SomeClass
 {
 public:
     void a();
     int b;
 };
 
 void testSingletonUsage()
 {
     // singleton<SomeClass> mySingleton; 
     // ���������� ������ ������� ����������, �.�.
     // ��������� �� ������.
     
     manual_singleton<SomeClass> myManualSingleton;
     myManualSingleton.doCreate(); // ������� ���������
     
     singleton<SomeClass> mySingleton; // ������ ����� ���������
     
     // ������ � ���������.
     mySingleton->a();
     mySingleton->b = 0;
 }
 \endcode
 */
template<class T>
class singleton : protected manual_singleton<T>
{
public:
    //! ������� ���������� ���� ��������� �� ����������.
    inline singleton()
    {
        if (!manual_singleton<T>::exists())
            throw std::logic_error("Instance doesn't exist");
        manual_singleton<T>::incRefCount();
    }
    
    //! ������ � ����������.
    inline T * operator ->()
    {
        return manual_singleton<T>::operator ->();
    }
    const inline T * operator ->() const
    {
        return manual_singleton<T>::operator ->();
    }
    
    // �������� �� ������������� � �� ������������ ����������.
    using manual_singleton<T>::exists;
    using manual_singleton<T>::unique;
};

} // namespace utility

#endif // _TANITA2_UTILITY_SINGLETON_H_
