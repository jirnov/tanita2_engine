/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 »сточник данных дл€ декодера.
 */
#pragma once
#ifndef _FLAMENCO_CORE_SOURCE_H_
#define _FLAMENCO_CORE_SOURCE_H_

namespace flamenco
{

// Ѕазовый класс дл€ источников данных дл€ декодера.
// ¬ случае ошибок бросает исключени€-наследники std::exception.
class source : noncopyable
{
public:
    virtual ~source() = 0 {}
    
    //  опирует count элементов размером size в destination. 
    // ¬ случае ошибки возращает ноль, иначе количество прочитанных элементов
    virtual u32 read( u8 * destination, u32 size, u32 count ) = 0;

    // ѕропускает определенное количество байтов.
    // offset может иметь отрицательное значение.
    virtual void skip( s32 offset ) = 0;

    // ”станавливает курсор чтени€ offset от начала потока
    virtual void seek( u32 offset ) = 0;

    // ¬озвращает текущее смещение курсора чтени€
    virtual u32 tell() const = 0;

    // ¬озвращает true, если поток подошел к концу
    virtual bool eof() const = 0;

    // ¬озвращает размер потока в байтах, ноль если размер определить нельз€
    u32 size() const { return mSize; }

protected:
    u32 mSize;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_SOURCE_H_
