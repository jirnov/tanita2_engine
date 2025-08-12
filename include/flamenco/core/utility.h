/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Вспомогательные классы.
 */
#pragma once
#ifndef _FLAMENCO_CORE_UTILITY_H_
#define _FLAMENCO_CORE_UTILITY_H_

#include <cassert>
#include <stdexcept>
#include <math.h>

namespace flamenco
{

// Идиома "noncopyable".
class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    noncopyable( const noncopyable & );
    noncopyable & operator =( const noncopyable & );
};

// Статический assert.
template<bool> struct STATIC_ASSERTION_FAILURE;
template<> struct STATIC_ASSERTION_FAILURE<true> {};
#define FLAMENCO_STATIC_ASSERT(expr) \
    typedef char static_assertion_failure_[sizeof(::flamenco::STATIC_ASSERTION_FAILURE<expr>)];


// Класс со встроенным счетчиком ссылок.
class refcountable : noncopyable
{
protected:
    refcountable()
        : mRefCount(0), mIsOk(MAGIC)
        {}
    virtual ~refcountable() = 0
    {
        assert(isOk());
        mIsOk = 0xdeadbeef; // Затираем метку.
        if (0 != mRefCount)
            throw std::logic_error("deleted refcountable has dangling references");
    }
    
protected:
    inline bool isOk() const
    {
        return MAGIC == mIsOk;
    }
    inline s32 refcount() const
    {
        return mRefCount;
    }
    
private:
    s32 mRefCount;
    u32 mIsOk;
    
    // Магическое число для проверки на затирание памяти.
    static const u32 MAGIC = 0xacceded;
    
    inline s32 incRef()
    {
        return ++mRefCount;
    }
    inline s32 decRef()
    {
        assert(mRefCount > 0);
        return --mRefCount;
    }
    template <class Refcountable> friend class reference;
};

// Умный указатель на refcountable.
template<class Refcountable>
class reference
{
public:
    explicit reference( Refcountable * object = NULL )
        : mObject(object)
    {
        if (NULL != object)
        {
            assert(object->isOk());
            assert(0 == object->refcount() && "Initializing with refcountable with dangling references");
            object->incRef();
        }
    }
    
    // Если не описать конструктор копирования, компилятор сгенерирует его сам,
    // причем вовсе не из шаблонного конструктора ниже :)
    reference( const reference & ref )
        : mObject(ref.mObject)
    {
        incRef();
    }
    
    template <class Y>
    reference( reference<Y> ref )
        : mObject(&*ref)
    {
        incRef();
    }
    
    reference & operator =( const reference & ref )
    {
        if (this != &ref)
        {
            decRef();
            mObject = ref.mObject;
            incRef();
        }
        return *this;
    }
    
    ~reference()
    {
        decRef();
    }
    
    operator bool() const
    {
        return NULL != mObject;
    }
    
    const Refcountable * operator ->() const
    {
        assert(NULL != mObject);
        assert(mObject->isOk());
        return mObject;
    }
    Refcountable * operator ->()
    {
        return const_cast<Refcountable *>(const_cast<const reference *>(this)->operator ->());
    }
    const Refcountable & operator *() const
    {
        assert(NULL != mObject);
        assert(mObject->isOk());
        return *mObject;
    }
    Refcountable & operator *()
    {
        return const_cast<Refcountable &>(const_cast<const reference *>(this)->operator *());
    }
    
    bool unique() const
    {
        return NULL != mObject && 1 == mObject->refcount();
    }
    
    bool operator ==( const reference & ref ) const
    {
        return mObject == ref.mObject;
    }
    
private:
    Refcountable * mObject;
    
    // Увеличение счетчика ссылок с проверками.
    void incRef()
    {
        if (NULL != mObject)
        {
            assert(mObject->isOk());
            mObject->incRef();
        }
    }
    
    // Уменьшение счетчика ссылок с корректным удалением.
    void decRef()
    {
        if (NULL == mObject)
            return;
        assert(mObject->isOk());
        if (0 == mObject->decRef())
        {
            delete mObject;
            mObject = NULL;
        }
    }
};

// RAII-объект блокировки.
template<class T, void (T::*lock)(), void (T::*unlock)()>
class scoped_lock : noncopyable
{
public:
    scoped_lock( T & object )
        : mObject(object)
    {
        (mObject.*lock)();
    }
    ~scoped_lock()
    {
        (mObject.*unlock)();
    }
    
protected:
    T & object()
    {
        return mObject;
    }
    
private:
    T & mObject;
};

// Умный указатель с семантикой владения и пользовательским deleter'ом.
template <typename T, template<typename> class deleter>
class auto_ptr
{
public:
    explicit auto_ptr( T * ptr = NULL )
        : mPtr(ptr)
        {}
    auto_ptr( auto_ptr & ptr )
        : mPtr(ptr.release())
        {}
    template <class U>
    auto_ptr( auto_ptr<U, deleter> & ptr )
        : mPtr(ptr.release())
        {}
    auto_ptr & operator =( auto_ptr & ptr )
    {
        reset(ptr.release());
    }
    ~auto_ptr()
    {
        reset(NULL);
    }
    
    const T * operator ->() const
    {
        assert(NULL != mPtr);
        return mPtr;
    }
    T * operator ->()
    {
        return const_cast<T *>(const_cast<const auto_ptr *>(this)->operator ->());
    }
    
    operator bool() const
    {
        return NULL != mPtr;
    }
    
    T * release()
    {
        T * ptr = mPtr;
        mPtr = NULL;
        return ptr;
    }
    const T * get() const
    {
        return mPtr;
    }
    T * get()
    {
        return mPtr;
    }
    void reset( T * ptr )
    {
        if (mPtr)
            deleter<T>()(mPtr);
        mPtr = ptr;
    }

private:
    T * mPtr;
};

// Deleter для массивов.
template <typename T>
struct array_deleter
{
    void operator ()( T * ptr )
    {
        delete [] ptr;
    }
};

// Умный указатель на массив.
template <typename T, template<typename T> class deleter=array_deleter>
class auto_array : public auto_ptr<T, deleter>
{
public:
    explicit auto_array( T * ptr = NULL )
        : auto_ptr<T, deleter>(ptr)
        {}
    const T & operator []( s32 index ) const
    {
        return *(get() + index);
    }
    T & operator []( s32 index )
    {
        return const_cast<T &>(const_cast<const auto_array *>(this)->operator [](index));
    }
};

// Заполнение звукового буфера тишиной.
template <typename T>
inline void set_silence( T * buffer, u32 sizeInSamples )
{
    memset(buffer, 0, sizeInSamples * sizeof(T));
}

// Приведение f32 к диапазону от 0 до 1.
template <typename T>
inline T clamp( T value, T minValue, T maxValue )
{
    return value < minValue ? minValue : value > maxValue ? maxValue : value;
}

// Сравнение двух float с некоторой погрешностью
inline bool float_equals( f32 a, f32 b, f32 EPSILON=0.05f )
{
    return (fabs(a - b) < EPSILON);
}

} // namespace flamenco

#endif // _FLAMENCO_CORE_UTILITY_H_
