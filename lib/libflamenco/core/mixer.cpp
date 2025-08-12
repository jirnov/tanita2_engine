/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 Реализация микшера.
 */
#include <flamenco/flamenco.h>
#include <algorithm>

using namespace flamenco;

// Критическая секция для доступа к микшеру.
critical_section mixer::mCriticalSection;

// Волшебное значение для проверки выхода за границы буферов.
const f32 mixer::MAGIC = 5.9742e24f; // Масса Земли :)

// Получение ссылки на единственную копию микшера.
mixer & mixer::singleton()
{
    static flamenco::mixer * mixer = NULL;
    if (NULL == mixer)
    {
        critical_section::lock lock(mCriticalSection);
        if (NULL == mixer)
            mixer = new flamenco::mixer();
    }
    return *mixer;
}

// Конструктор.
mixer::mixer()
{
    mBufferL.reset(reinterpret_cast<f32 *>(_aligned_malloc((CHANNEL_BUFFER_SIZE_IN_SAMPLES + 1) * sizeof(f32), 16)));
    if (NULL == mBufferL.get() || (reinterpret_cast<int>(mBufferL.get()) % 16 != 0))
        throw std::bad_alloc("failed aligned allocation mBufferL");

    mBufferR.reset(reinterpret_cast<f32 *>(_aligned_malloc((CHANNEL_BUFFER_SIZE_IN_SAMPLES + 1) * sizeof(f32), 16)));
    if (NULL == mBufferR.get() || (reinterpret_cast<int>(mBufferR.get()) % 16 != 0))
        throw std::bad_alloc("failed aligned allocation mBufferR");

    mMixerBuffer.reset(reinterpret_cast<f32 *>(_aligned_malloc((MIXER_BUFFER_SIZE_IN_SAMPLES + 1) * sizeof(f32), 16)));
    if (NULL == mMixerBuffer.get() || (reinterpret_cast<int>(mMixerBuffer.get()) % 16 != 0))
        throw std::bad_alloc("failed aligned allocation mMixerBuffer");

    // В конец буферов каналов запишем магическое значение,
    // чтобы отловить выход за границы памяти.
    mBufferL[CHANNEL_BUFFER_SIZE_IN_SAMPLES] = MAGIC;
    mBufferR[CHANNEL_BUFFER_SIZE_IN_SAMPLES] = MAGIC;
    mMixerBuffer[MIXER_BUFFER_SIZE_IN_SAMPLES] = MAGIC;
}

// Присоединение пина.
void mixer::attach( reference<pin> pin )
{
    critical_section::lock lock(mCriticalSection);
    assert(!pin->connected());
    pin->set_connected(true);
    pin->on_attach(true);
    mPins.push_back(pin);
}

// Отсоединение пина. Если он не присоединен, warning в лог.
void mixer::detach( reference<pin> pin )
{
    critical_section::lock lock(mCriticalSection);
    PinList::iterator i = std::find(mPins.begin(), mPins.end(), pin);
    if (mPins.end() == i)
        throw std::runtime_error("pin is not attached");
    assert((*i)->connected());
    (*i)->set_connected(false);
    (*i)->on_attach(false);
    mPins.erase(i);
}

// Микширование.
void mixer::mix( s16 * buffer )
{
    critical_section::lock lock(mCriticalSection);

    set_silence(mMixerBuffer.get(), MIXER_BUFFER_SIZE_IN_SAMPLES);

    for (PinList::iterator i = mPins.begin(); mPins.end() != i; ++i)
    {
        assert((*i)->connected());
        // Заполняем тишиной буферы левого и правого каналов.
        set_silence(mBufferL.get(), CHANNEL_BUFFER_SIZE_IN_SAMPLES);
        set_silence(mBufferR.get(), CHANNEL_BUFFER_SIZE_IN_SAMPLES);

        // Заполняем каналы.
        (*i)->process(mBufferL.get(), mBufferR.get());
        // Проверяем выход за границы.
        assert(MAGIC == mBufferL[CHANNEL_BUFFER_SIZE_IN_SAMPLES] &&
               MAGIC == mBufferR[CHANNEL_BUFFER_SIZE_IN_SAMPLES]);

        __m128 *ptrL = reinterpret_cast<__m128 *>(mBufferL.get()),
               *ptrR = reinterpret_cast<__m128 *>(mBufferR.get()),
               *mixerPtrL = reinterpret_cast<__m128 *>(mMixerBuffer.get()),
               *mixerPtrR = reinterpret_cast<__m128 *>(mMixerBuffer.get() + MIXER_BUFFER_SIZE_IN_SAMPLES / 2);

        // Примешиваем данные в итоговый буфер.
        for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES / 4; ++i)
        {           
            *mixerPtrL++ = _mm_add_ps(*mixerPtrL, _mm_mul_ps(*ptrL++, _mm_set1_ps(0.33f)));
            *mixerPtrR++ = _mm_add_ps(*mixerPtrR, _mm_mul_ps(*ptrR++, _mm_set1_ps(0.33f)));
        }
    }
    // Проверка выхода за пределы массива
    assert(MAGIC == mMixerBuffer[MIXER_BUFFER_SIZE_IN_SAMPLES]);

    // Заполняем выходной буфер тишиной.
    set_silence(buffer, MIXER_BUFFER_SIZE_IN_SAMPLES);

    s16 *ptrL = buffer,
        *ptrR = buffer + 1;
    f32 *mixerPtrL = mMixerBuffer.get(),
        *mixerPtrR = mMixerBuffer.get() + MIXER_BUFFER_SIZE_IN_SAMPLES / 2;

    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i, ++ptrL, ++ptrR)
    {
        *ptrL++ = static_cast<s16>(*mixerPtrL++ * (1 << 15));
        *ptrR++ = static_cast<s16>(*mixerPtrR++ * (1 << 15));
    }
}

// Деструктор. Нужен для отсоединения пинов.
mixer::~mixer()
{
    critical_section::lock lock(mCriticalSection);
    for (PinList::iterator i = mPins.begin(); mPins.end() != i; ++i)
    {
        assert((*i)->connected());
        (*i)->set_connected(false);
    }
}
