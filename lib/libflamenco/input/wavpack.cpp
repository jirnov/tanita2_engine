/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация источника звука из wavpack-файла.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

namespace
{

// Чтение из источника.
int32_t read_bytes( void * id, void * data, int32_t bcount )
{
    return reinterpret_cast<source *>(id)->read(reinterpret_cast<u8 *>(data), 1, bcount);
}

// Получение позиции чтения.
uint32_t get_pos( void * id )
{
    return reinterpret_cast<source *>(id)->tell();
}

// Установка абсолютного смещения.
int32_t set_pos_abs( void * id, uint32_t pos )
{
    reinterpret_cast<source *>(id)->seek(pos);
    return 0;
}

// Установка относительного смещения.
int32_t set_pos_rel( void * id, int32_t delta, int32_t mode )
{
    source * input = reinterpret_cast<source *>(id);

    switch(mode)
    {
    case SEEK_SET:
        input->seek(delta);
        break;
    case SEEK_END:
        input->seek(input->size());
        break;
    case SEEK_CUR:
        input->skip(delta);
        break;
    }
    return 0;
}

// Перемещение на один символ назад (ungetc).
int32_t push_back_byte( void * id, int32_t c )
{
    reinterpret_cast<source *>(id)->skip(-1);
    return c;
}

// Получение длины потока.
uint32_t get_length( void * id )
{
    return reinterpret_cast<source *>(id)->size();
}

// Возможно ли перемещение по файлу.
int32_t can_seek( void * id )
{
    return reinterpret_cast<source *>(id)->size() != 0;
}

// Запись в поток, не поддерживается.
int32_t write_bytes( void *, void *, int32_t )
{
    return 0;
}

// Функции для чтения данных декодером.
WavpackStreamReader gCallbacks = {
    read_bytes,
    get_pos,
    set_pos_abs,
    set_pos_rel,
    push_back_byte,
    get_length, 
    can_seek,
    write_bytes
};

// Магическое значение для проверки выхода за границы буфера.
const u32 MAGIC = 0x900d900d;

} // namespace

// Конструктор.
wavpack_decoder::wavpack_decoder( std::auto_ptr<source> source )
    : mSource(source), mSampleRate(0), mSampleCount(0),
      mChannelCount(0), mBuffer(NULL), mBufferSize(0),
      mBufferFilledSize(0), mBufferOffset(0)
{
    assert(mSource.get());

    // Буфер для ошибок при разборе wv-файла, размер в 80 символов - из документации.
    char errorBuffer[80];
    
    mWavpackFile.reset(WavpackOpenFileInputEx(&gCallbacks, mSource.get(), NULL, errorBuffer, 0, 0));
    if (!mWavpackFile)
        throw std::runtime_error(errorBuffer);

    // Получаем количество каналов.
    mChannelCount = WavpackGetNumChannels(mWavpackFile.get());
    if (mChannelCount > 2)
        throw std::runtime_error("Expected mono or stereo stream.");
    
    // Проверяем битность.
    if (WavpackGetBitsPerSample(mWavpackFile.get()) != 16)
        throw std::runtime_error("Unsupported sample format (expected 16 bits).");
    
    // Получаем частоту.
    mSampleRate = WavpackGetSampleRate(mWavpackFile.get());

    // Длина звука в семплах.
    mSampleCount = WavpackGetNumSamples(mWavpackFile.get());
    
    // Буфер декодирования.
    const u32 BUFFER_SIZE = mSampleRate * DECODE_BUFFER_SIZE_IN_MSEC / 1000;
    mBufferSize = (mSampleCount < BUFFER_SIZE ? mSampleCount : BUFFER_SIZE) * mChannelCount;
    mBuffer.reset(new s32[mBufferSize + 1]);
    mBuffer[mBufferSize] = MAGIC; // Значение для проверки выхода за границы.
}

// Декодирование потока во внутренний буфер. Возвращает количество декодированных семплов.
u32 wavpack_decoder::unpack_to_buffer()
{
    return WavpackUnpackSamples(mWavpackFile.get(), reinterpret_cast<int32_t *>(mBuffer.get()),
                                mBufferSize / mChannelCount) * mChannelCount;
}

// Установка курсора начала декодирования на заданный семпл.
void wavpack_decoder::seek( u32 sample )
{
    // Если ошибка, значит надо открывать файл заново!
    if (!WavpackSeekSample(mWavpackFile.get(), sample))
    {
        // Закрытие файла.
        mWavpackFile.reset(NULL);

        // Перемотка источника на начало.
        mSource->seek(0);

        // Открытие файла заново.
        char errorBuffer[80];
        mWavpackFile.reset(WavpackOpenFileInputEx(&gCallbacks, mSource.get(), NULL, errorBuffer, 0, 0));
        if (!mWavpackFile)
            throw std::runtime_error(errorBuffer);

        // Попытка сделать seek в заново открытом файле.
        if (!WavpackSeekSample(mWavpackFile.get(), sample))
            throw std::runtime_error("Seek failed");
    }

    // Сбрасываем указатель на текущий семпл
    mBufferOffset = 0;
    mBufferFilledSize = 0;
}

// Копирует в левый и правый каналы count декодированных семплов.
u32 wavpack_decoder::unpack( f32 * left, f32 * right, u32 count )
{
    s32 * ptr = mBuffer.get() + mBufferOffset;
    
    u32 sampleCount = 0;
    while (sampleCount < count)
    {
        if (mBufferOffset >= mBufferFilledSize)
        {
            assert(mBufferOffset == mBufferFilledSize);
            
            // Пытаемся подгрузить еще данных в буфер
            mBufferOffset = 0;
            ptr = mBuffer.get();
            if (0 == (mBufferFilledSize = unpack_to_buffer()))
                break;
        }
        f32 sample = *left++ = *ptr++ / static_cast<f32>(1 << 15);
        *right++ = (mChannelCount == 1) ? sample : *ptr++ / static_cast<f32>(1 << 15);
        mBufferOffset += mChannelCount;
        sampleCount++;
    }
    assert(mBuffer[mBufferSize] == MAGIC); // Проверяем выход за границы буфера.
    return sampleCount;
}
