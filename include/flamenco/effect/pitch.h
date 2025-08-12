/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Эффект для изменения громкости и панорамирования.
 */
#pragma once
#ifndef _FLAMENCO_EFFECT_PITCH_H_
#define _FLAMENCO_EFFECT_PITCH_H_

namespace flamenco
{

// Эффект для изменения скорости проигрывания звука.
class pitch : public effect
{
public:
    // Создание нового эффекта. Скорость задается от 0.25 до 4.0,
    // 0.25 - уменьшение скорости в четыре раза
    // 4.0  - увеличение скорости в четыре раза
    static reference<pitch> create( reference<pin> input, f32 pitch_value=1.0f );
    
    // Скорость в диапазоне от 0.25 до 4.0.
    atomic<f32> pitch_value;
    
private:
    pitch( reference<pin> input, f32 pitch_value );
    
    // Заполняет буферы каналов звуковыми данными.
    void process( f32 * left, f32 * right );

    // Смещение внутри буферов
    u32 mBufferOffset;

    // Размер буфера
    u32 mBufferSize;

    template <class T>
    struct aligned_deleter
    {
        void operator()(f32 * ptr)
        {
            _aligned_free(ptr);
        }
    };
    
    // Внутренние буферы для каналов
    auto_array<f32, aligned_deleter> mBufferL, mBufferR;

    // Волшебное значение для проверки выхода за границы буферов.
    static const f32 MAGIC;
};

} // namespace flamenco

#endif // _FLAMENCO_EFFECT_PITCH_H_
