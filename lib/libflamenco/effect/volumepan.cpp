/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация эффекта для изменения громкости и панорамирования.
 */
#include <flamenco/flamenco.h>
#include <xmmintrin.h>

using namespace flamenco;

namespace {

f32 clamp_0_1( f32 value )
{
    return clamp(value, 0.0f, 1.0f);
}
 
}

// Создание нового эффекта.
reference<volume_pan> volume_pan::create( reference<pin> input, f32 volume, f32 pan )
{
    return reference<volume_pan>(new volume_pan(input, volume, pan));
}

// Конструктор.
volume_pan::volume_pan( reference<pin> input, f32 volume, f32 pan )
    : effect(input), volume(volume), pan(pan)
{
    assert(0.0f <= volume && volume <= 1.0f);
    assert(-1.0f <= pan && pan <= 1.0f);
}

// Заполняет буферы каналов звуковыми данными.
void volume_pan::process( f32 * left, f32 * right )
{
    const f32 volume = clamp_0_1(this->volume()), pan = this->pan();

    process_input_pin(left, right);

    if (float_equals(volume, 1.0f) && float_equals(pan, 0.0f))
        return;

    const __m128 volumeLeft = _mm_set1_ps(volume * clamp_0_1(1.0f - pan));
    const __m128 volumeRight = _mm_set1_ps(volume * clamp_0_1(1.0f + pan));

    __m128 * leftPtr = reinterpret_cast<__m128 *>(left);
    __m128 * rightPtr = reinterpret_cast<__m128 *>(right);

    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES / 4; ++i)
    {
        *leftPtr++ = _mm_mul_ps(*leftPtr, volumeLeft);
        *rightPtr++ = _mm_mul_ps(*rightPtr, volumeRight);
    }
}
