/*
 Движок "Танита2".
 Шаблоны синглтонов.
 */
#pragma once
#ifndef _TANITA2_UTILITY_SINGLETON_H_
#define _TANITA2_UTILITY_SINGLETON_H_

#include <boost/noncopyable.hpp>

namespace utility
{

//! Шаблон синглтона с созданием экземпляра вручную.
/**
 Реализация синглтона с созданием экземпляра класса-
 параметра T вручную (через метод doCreate()).
 Необходим для обеспечения правильного порядка создания
 экземпляров разных классов.
 
 Использование:
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
     
     // Обращения вроде mySingleton->a() приведут к
     // исключению, т.к. экземпляр еще не создан.
     
     mySingleton.doCreate(); // Создание экземпляра
     
     // Доступ к свойствам
     mySingleton->a();
     mySingleton->b = 0;
     
     // Если синглтон больше никем не используется,
     // он будет удален автоматически.
 }
 \endcode
 */
template<class T>
class manual_singleton : boost::noncopyable
{
public:
    // Конструктор по умолчанию.
    manual_singleton() {}
    
    //! Создание экземпляра.
    /**
     Бросает исключение если экземпляр уже существует или когда-то существовал,
     но был удален.
     */
    void doCreate()
    {
        checkBeforeCreation();
        initialize(new T());
    }
    
    //! Создание экземпляра с инициализацией создаваемого объекта.
    /**
     Создает экземпляр класса T вызовом конструктора от переданного initializer.
     Бросает исключение если экземпляр уже существует или когда-то 
     существовал, но был удален.
     */
    template<class Initializer>
    void doCreate( const Initializer & initializer )
    {
        checkBeforeCreation();
        initialize(new T(initializer));
    }
    
    //! Уменьшение счетчика и удаление экземпляра, если счетчик занулен.
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
    
    //! Доступ к экземпляру.
    const T * operator ->() const
    {
        assert(NULL != mInstance);
        return mInstance;
    }
    T * operator ->()
    {
        return const_cast<T *>(const_cast<const manual_singleton *>(this)->operator ->());
    }
    
    //! Проверка на единственность экземпляра.
    static bool unique()
    {
        assert(mInstance);
        return (1 == mRefCount);
    }
    
    //! Проверка существования экземпляра.
    static bool exists()
    {
        return NULL != mInstance;
    }
    
protected:
    // Увеличение счетчика ссылок.
    void incRefCount()
    {
        assert(mInstance);
        mRefCount++;
    }
    
private:
    // Проверка правильности внутренних данных.
    void checkBeforeCreation() const
    {
        if (NULL != mInstance)
            throw std::logic_error("Singleton instance already exists");
        if (mIsCreatedSecondTime)
            throw std::logic_error("Singleton instance created second time");
    }
    // Инициализация класса.
    void initialize( T * newptr )
    {
        assert(NULL != newptr);
        mInstance = newptr;
        mIsCreatedSecondTime = true;
        mRefCount++;
    }
    
    // Счетчик ссылок.
    static s32 mRefCount;
    // Указатель на экземпляр.
    static T * mInstance;
    // Флаг для проверки повторного создания.
    static bool mIsCreatedSecondTime;
};

// Счетчик ссылок.
template<class T> s32 manual_singleton<T>::mRefCount;
// Указатель на экземпляр.
template<class T> T * manual_singleton<T>::mInstance;
// Флаг для проверки повторного создания.
template<class T> bool manual_singleton<T>::mIsCreatedSecondTime = false;


//! Шаблон синглтона, не создающего экземпляр.
/**
 Реализация синглетона, не создающая экземпляр, если
 он уже не существует. Для создания экземпляра должен
 использоваться manual_singleton.
 
 Пример использования:
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
     // Предыдущая строка вызовет исключение, т.к.
     // экземпляр не создан.
     
     manual_singleton<SomeClass> myManualSingleton;
     myManualSingleton.doCreate(); // Создаем экземпляр
     
     singleton<SomeClass> mySingleton; // Теперь можно объявлять
     
     // Доступ к свойствам.
     mySingleton->a();
     mySingleton->b = 0;
 }
 \endcode
 */
template<class T>
class singleton : protected manual_singleton<T>
{
public:
    //! Бросает исключение если экземпляр не существует.
    inline singleton()
    {
        if (!manual_singleton<T>::exists())
            throw std::logic_error("Instance doesn't exist");
        manual_singleton<T>::incRefCount();
    }
    
    //! Доступ к экземпляру.
    inline T * operator ->()
    {
        return manual_singleton<T>::operator ->();
    }
    const inline T * operator ->() const
    {
        return manual_singleton<T>::operator ->();
    }
    
    // Проверка на существование и на уникальность экземпляра.
    using manual_singleton<T>::exists;
    using manual_singleton<T>::unique;
};

} // namespace utility

#endif // _TANITA2_UTILITY_SINGLETON_H_
