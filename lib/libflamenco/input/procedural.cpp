/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация процедурных источников звука.
 */
#include <flamenco/flamenco.h>
#define _USE_MATH_DEFINES // Включение макросов-определений (M_PI)
#include <cmath>

using namespace flamenco;

// Конструктор.
sine::sine( u32 frequency )
    : frequency(frequency), mTimeShift(0)
{
}

// Заполнение буфера гармоническим звуком.
void sine::process( f32 * left, f32 * right )
{
    u32 frequency = this->frequency();
    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        f32 value = sinf(2 * static_cast<f32>(M_PI) * frequency * (mTimeShift + i) / FREQUENCY);
        *left++ = *right++ = value;
    }
    mTimeShift = (mTimeShift + CHANNEL_BUFFER_SIZE_IN_SAMPLES) % FREQUENCY;
}

// Создание нового источника гармонического звука с частотой frequency Гц.
reference<sine> sine::create( u32 frequency )
{
    return reference<sine>(new sine(frequency));
}


// Заполнение каналов случайными числами.
void noise::process( f32 * left, f32 * right )
{
    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        *left++  = static_cast<f32>(rand() - (RAND_MAX >> 1)) / (RAND_MAX >> 1);
        *right++ = static_cast<f32>(rand() - (RAND_MAX >> 1)) / (RAND_MAX >> 1);
    }
}

// Создание источника шума
reference<noise> noise::create()
{
    return reference<noise>(new noise());
}
