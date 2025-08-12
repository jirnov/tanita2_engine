/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 Источник звуковых данных из файла или иного источника данных.
 */
#pragma once
#ifndef _FLAMENCO_CORE_STREAM_H_
#define _FLAMENCO_CORE_STREAM_H_

namespace flamenco
{

// Базовый класс для звуковых потоков.
// Нужен для того, чтобы к потокам разных типов можно было обращаться
// по общему интерфейсу.
class sound_stream_base : public pin
{
public:
    // Флаг зацикленности звука.
    atomic<bool> looping;

    // Проигрывается ли звук (сбрасывается в false, когда незацикленный звук подошел к концу).
    atomic_readonly<bool, sound_stream_base> playing;

    // Количество циклов, которые успел проиграть звук. Если звук незациклен, счетчик равен 0.
    atomic_readonly<u32, sound_stream_base> loop_count;

    // Сколько времени длится звук.
    virtual f32 time_total() const = 0;

    // Сколько времени прошло с начала звука.
    virtual f32 time_elapsed() const = 0;

    // Установка курсора чтения на определенный семпл с учетом количества каналов
    virtual void seek( u32 sample ) = 0;

    // Номер текущего проигрываемого семпла с учетом количества каналов.
    virtual u32 tell() const = 0;

    // Длина звукового потока в семплах с учетом количества каналов.
    virtual u32 size() const = 0;

protected:
    sound_stream_base()
        : looping(false), playing(true), loop_count(0), mPosition(0)
        {}

    // Установка флагов. Функции нужны, т.к. переменные имеют приватный set().
    inline bool set_playing( bool value )
    {
        return playing.set(value);
    }
    inline u32 set_loop_count( u32 loopCount )
    {
        return loop_count.set(loopCount);
    }
    inline u32 set_position( u32 pos )
    {
        return mPosition.set(pos);
    }

    // Номер текущего проигрываемого семпла (с точностью до LATENCY).
    atomic_readonly<u32, sound_stream_base> mPosition;

private:
    // Устанавливаем флаг playing во время присоединения цепочки к микшеру.
    inline void on_attach(bool connected)
    {
        set_playing(connected && (tell() < size() || looping()));
    }
};

/*
 Все декодеры должны следовать одному интерфейсу:
 struct decoder
 {
     // Конструктор.
     decoder( std::auto_ptr<source> source );

     // Копирует в левый и правый каналы count декодированных семплов.
     // Возвращает количество скопированных семплов, оно может быть меньше count,
     // если поток закончился.
     u32 unpack( f32 * left, f32 * right, u32 count );

     // Установка курсора начала декодирования на заданный семпл.
     void seek( u32 sample );

     // Длина потока в семплах. Функция оптимизирована для частых вызовов.
     u32 length() const;

     // Количество каналов.
     u32 channels() const;

     // Частота звукового потока (для преобразования частоты потоком).
     u32 frequency() const;
 }
 */

// Источник звуковых данных из файлов различных форматов.
template <class decoderT>
class stream : public sound_stream_base
{
public:
    // Создание потока.
    static reference<stream> create( std::auto_ptr<source> source );

private:
    stream( std::auto_ptr<source> source )
        : mDecoder(source), mBufferL(NULL), mBufferR(NULL)
    {
        mBufferSizeInSamples = static_cast<u32>(CHANNEL_BUFFER_SIZE_IN_SAMPLES * static_cast<f32>(mDecoder.frequency()) / FREQUENCY);

        // Создаем буферы для нестандартной частоты
        if (!is_standard_frequency(mDecoder.frequency()))
        {
            mBufferL.reset(new f32[mBufferSizeInSamples]);
            mBufferR.reset(new f32[mBufferSizeInSamples]);
        }
    }

    // Заполняет буферы каналов звуковыми данными, полученными от декодера.
    // Если частота данных декодера не совпадает с частотой библиотеки,
    // выполняет преобразование.
    void process( f32 * left, f32 * right );

    // Сколько времени длится звук
    inline f32 time_total() const
    {
        return mDecoder.length() / static_cast<f32>(mDecoder.frequency());
    }

    // Сколько времени прошло с начала звука
    inline f32 time_elapsed() const
    {
        return mPosition() / static_cast<f32>(mDecoder.frequency());
    }

    // Длина одного канала в семплах.
    inline u32 size() const
    {
        return mDecoder.length() / mDecoder.channels();
    }

    // Установка курсора чтения на определенный семпл.
    inline void seek( u32 sample )
    {
        set_position(sample * mDecoder.channels());
        mDecoder.seek(sample * mDecoder.channels());
    }

    // Получение текущего семпла.
    inline u32 tell() const
    {
        return mPosition() / mDecoder.channels();
    }

    // Требуется ли преобразование частоты
    inline bool is_standard_frequency( u32 freq ) const
    {
        return freq == FREQUENCY;
    }

    // Преобразование частоты
    void convert_frequency(f32 * dst, u32 dst_size, const f32 * src, u32 src_size) const
    {
        const f32 rate = static_cast<f32>(src_size) / dst_size;
        for (f32 pos = 0.0, *ptr = dst; pos < src_size; pos += rate)
            *ptr++ = src[static_cast<u32>(pos)];
    }

    // Внутренние буферы для нестандартных частот (не 44100).
    auto_array<f32> mBufferL, mBufferR;

    // Размер внутренних буферов в семплах.
    u32 mBufferSizeInSamples;

    // Декодер.
    decoderT mDecoder;
};


// Реализация.

template <class decoderT>
reference<stream<decoderT> > stream<decoderT>::create( std::auto_ptr<source> source )
{
    return reference<stream<decoderT> >(new stream<decoderT>(source));
}

template <class decoderT>
void stream<decoderT>::process( f32 * left, f32 * right )
{
    bool looping = this->looping();

    // Если частота стандартная, то распаковка идет напрямую в left и right.
    f32 *bufferL = (is_standard_frequency(mDecoder.frequency())) ? left : mBufferL.get(),
        *bufferR = (is_standard_frequency(mDecoder.frequency())) ? right : mBufferR.get();

    // Заполняем каналы данными. Если скопировано меньше, чем нужно, значит поток закончился.
    u32 count = mDecoder.unpack(bufferL, bufferR, mBufferSizeInSamples);
    if (count < mBufferSizeInSamples)
    {
        if (looping)
        {
            // Заполняем буфер, читая файл снова и снова.
            u32 loopCount = 0, samplesRead = 0;
            while (count < mBufferSizeInSamples)
            {
                mDecoder.seek(0);
                samplesRead = mDecoder.unpack(bufferL + count, bufferR + count, mBufferSizeInSamples - count);
                count += samplesRead;
                loopCount++;
            }
            assert(count == mBufferSizeInSamples);
            set_loop_count(loop_count() + loopCount);
            set_position(samplesRead);
        }
        else // Файл доиграл до конца, выставляем флаги.
        {
            if (playing())
                set_loop_count(loop_count() + 1);
            set_playing(false);
        }
    }
    else
    {
        assert(count == mBufferSizeInSamples);
        set_position(mPosition() + mBufferSizeInSamples);
        set_playing(true);
    }

    if (!playing())
        return;

    // Нестандартная частота
    if (!is_standard_frequency(mDecoder.frequency()))
    {
        convert_frequency(left, CHANNEL_BUFFER_SIZE_IN_SAMPLES, bufferL, mBufferSizeInSamples);
        convert_frequency(right, CHANNEL_BUFFER_SIZE_IN_SAMPLES, bufferR, mBufferSizeInSamples);
    }
}

} // namespace flamenco

#endif // _FLAMENCO_CORE_STREAM_H_
