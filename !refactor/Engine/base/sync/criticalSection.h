/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef __BASE_SYNC_CRITICALSECTION_H__
#define __BASE_SYNC_CRITICALSECTION_H__

/**
 Работа с критическими секциями.
 Для работы с критическими секциями предназначены
 два класса - CriticalSection и Lock.
 Первый представляет собой оболочку над платформо-
 зависимой реализацией критических секций и 
 позволяет создавать объект для входа в секцию.
 
 Второй представляет собой реализацию RAII, осуществляя
 вход в критическую секцию при создании и автоматически
 выход из нее при удалении.
 */

namespace base {
namespace sync
{

//
// Классы для работы с критическими секциями.
//

/**
 Класс для представления критической секции и создания
 объектов входа в нее типа Lock.
 */
class CriticalSection
{
public:
    // Тип объекта блокировки.
    typedef class Lock lock_type;
    
    
    // Создание критической секции.
    CriticalSection();
    // Удаление критической секции.
    ~CriticalSection();
    
    // Создание объекта Lock для входа в критическую секцию.
    lock_type doEnter();
    
    
private:
    // Критическая секция
    CRITICAL_SECTION mCriticalSection;
    
    // Запрещенные конструктор и оператор присваивания
    CriticalSection( const CriticalSection & );
    void operator =( const CriticalSection & );
};

/**
 Класс для входа в критическую секцию с автоматическим 
 выходом при удалении (идиома RAII - resource acquisition 
 is initialization).
 */
class Lock
{
public:
    // Перехватывает право на секцию (без входа в нее) у другого объекта.
    Lock( Lock & );
    // Выход из секции, если она не принадлежит кому-то другому.
    ~Lock();
    

protected:
    // Конструктор по умолчанию (для возможности наследования).
    Lock();
    
    
private:
    // Вход в критическую секцию.
    explicit Lock( CRITICAL_SECTION * );
    
    
    // Флаг владения секцией (определяет необходимость выхода из нее в деструкторе).
    bool mOwn;
    // Указатель на критическую секцию.
    CRITICAL_SECTION * mCriticalSectionPtr;
    
    
    // Оператор присваивания запрещен
    void operator =( const Lock & );
    
    friend Lock CriticalSection::doEnter();
};


//
// Классы для имитации работы с критическими секциями.
// (для использования в шаблонах, условно поддерживающих блокировки)
//

/* Класс-имитатор критической секции. */
class DummyCriticalSection
{
public:
    // Тип объекта блокировки.
    typedef class DummyLock lock_type;
    
    // Имитация создания блокировки. Возвращает объект DummyLock.
    lock_type doEnter();
};

/* Класс-имитатор входа в критическую секцию (типа DummyCriticalSection). */
class DummyLock {};

}} // base::sync namespace

#endif // __BASE_SYNC_CRITICALSECTION_H__
