/*
 Движок "Танита2".
 Классы для объекта со счетчиком ссылок и умного указателя на него.
 */
#pragma once
#ifndef _TANITA2_UTILITY_REFCOUNTABLE_H_
#define _TANITA2_UTILITY_REFCOUNTABLE_H_

#include <boost/noncopyable.hpp>

namespace utility
{

// Класс для объекта со встроенным счетчиком ссылок.
class refcountable : boost::noncopyable
{
protected:
    refcountable()
        : mRefCount(0), mIsInvalid(false)
        {}
    virtual ~refcountable()
    {
        assert(false == mIsInvalid);
        mIsInvalid = true;
        if (0 != mRefCount)
            throw std::logic_error("refcountable object being deleted has dangling references");
    }
    
protected:
    inline bool isOk() const
    {
        return !mIsInvalid;
    }
    inline s32 refcount() const
    {
        return mRefCount;
    }
    
private:
    s32 mRefCount;
    bool mIsInvalid; // Проверяем на false, т.к. true более вероятно при затирании.
    
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
    template <class Refcountable> friend class weakreference;
};

// Класс для умного указателя на refcountable.
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

// Класс для слабой ссылки на refcountable.
template<class Refcountable>
class weakreference
{
public:
    explicit weakreference( Refcountable * object = NULL )
        : mObject(object)
        {}
    
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
        return const_cast<Refcountable *>(const_cast<const weakreference *>(this)->operator ->());
    }
    const Refcountable & operator *() const
    {
        assert(NULL != mObject);
        assert(mObject->isOk());
        return *mObject;
    }
    Refcountable & operator *()
    {
        return const_cast<Refcountable &>(const_cast<const weakreference *>(this)->operator *());
    }
    
    bool operator ==( const weakreference & ref ) const
    {
        return mObject == ref.mObject;
    }
    
private:
    Refcountable * mObject;
};

} // namespace utility

#endif // _TANITA2_UTILITY_REFCOUNTABLE_H_
