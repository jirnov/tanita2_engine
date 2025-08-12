/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация эффекта для изменения скорости проигрывания звука.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// Волшебное значение для проверки выхода за границы буферов.
const f32 pitch::MAGIC = 5.9742e24f; // Масса Земли :)

// Создание нового эффекта.
reference<pitch> pitch::create( reference<pin> input, f32 pitch_value )
{
    return reference<pitch>(new pitch(input, pitch_value));
}

// Конструктор.
pitch::pitch( reference<pin> input, f32 pitch_value )
    : effect(input), pitch_value(pitch_value),
      mBufferSize(0),
      mBufferOffset(0)
{
    mBufferL.reset(reinterpret_cast<f32 *>(_aligned_malloc((CHANNEL_BUFFER_SIZE_IN_SAMPLES + 1) * sizeof(f32), 16)));
    if (NULL == mBufferL.get() || (reinterpret_cast<int>(mBufferL.get()) % 16 != 0))
        throw std::bad_alloc("failed aligned allocation mBufferL");

    mBufferR.reset(reinterpret_cast<f32 *>(_aligned_malloc((CHANNEL_BUFFER_SIZE_IN_SAMPLES + 1) * sizeof(f32), 16)));
    if (NULL == mBufferR.get() || (reinterpret_cast<int>(mBufferR.get()) % 16 != 0))
        throw std::bad_alloc("failed aligned allocation mBufferR");

    mBufferL[CHANNEL_BUFFER_SIZE_IN_SAMPLES] = MAGIC;
    mBufferR[CHANNEL_BUFFER_SIZE_IN_SAMPLES] = MAGIC;
    assert(0.25f <= pitch_value && pitch_value <= 4.0f);
}

// Заполняет буферы каналов звуковыми данными.
void pitch::process( f32 * left, f32 * right )
{
    const f32 pitch_value = clamp(this->pitch_value(), 0.25f, 4.0f);

    if (float_equals(pitch_value, 1.0f, 0.025f))
    {
        process_input_pin(left, right);
    }
    else
    {
        u32 sampleCount = 0;

        while (sampleCount < CHANNEL_BUFFER_SIZE_IN_SAMPLES)
        {
            bool doProcessInputPin = true;

            for (f32 pos = static_cast<f32>(mBufferOffset); pos < mBufferSize; pos += pitch_value)
            {
                mBufferOffset = static_cast<u32>(pos);

                *left++ = mBufferL[mBufferOffset];
                *right++ = mBufferR[mBufferOffset];

                if (++sampleCount == CHANNEL_BUFFER_SIZE_IN_SAMPLES)
                {
                    doProcessInputPin = false;
                    break;
                }
            }
    
            if (doProcessInputPin)
            {
                process_input_pin(mBufferL.get(), mBufferR.get());
                mBufferOffset = 0;
                mBufferSize = CHANNEL_BUFFER_SIZE_IN_SAMPLES;
            }
        }

        assert(MAGIC == mBufferL[CHANNEL_BUFFER_SIZE_IN_SAMPLES] &&
               MAGIC == mBufferR[CHANNEL_BUFFER_SIZE_IN_SAMPLES]);
    }
}
