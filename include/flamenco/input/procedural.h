/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Процедурные источники звука.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_PROCEDURAL_H_
#define _FLAMENCO_INPUT_PROCEDURAL_H_

namespace flamenco
{

// Источник гармонического звука.
class sine : public pin
{
public:
    // Создание нового источника гармонического звука с частотой frequency Гц.
    static reference<sine> create( u32 frequency );
    
    
    // Частота синусоиды.
    atomic<u32> frequency;
    
private:
    sine( u32 frequency );
    
    // Заполняет буферы каналов звуковыми данными.
    void process( f32 * left, f32 * right );
    
    // Временной сдвиг синусоиды.
    u32 mTimeShift;
    
    // frequency сделана публичной переменной без get/set, т.к. atomic_u32 уже
    // содержит свои get и set, код был бы похож на лапшу.
};

// Источник белого шума.
class noise : public pin
{
public:
    // Создание источника белого шума.
    static reference<noise> create();
    
private:
    noise() {}
    
    // Заполняет буферы каналов случайными значениями.
    void process( f32 * left, f32 * right );
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_PROCEDURAL_H_
