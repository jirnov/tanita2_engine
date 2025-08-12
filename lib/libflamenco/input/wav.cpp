/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация источника звука из wav-файла.
 */
#include <flamenco/flamenco.h>
#pragma warning(disable:4201) // Отключение предупреждения о nameless struct.
#include <mmsystem.h>

#include <iostream>

using namespace flamenco;

namespace 
{

// Магическое значение для проверки выхода за границы буфера.
const s16 MAGIC = 0x9d;

}

// Конструктор.
wav_decoder::wav_decoder( std::auto_ptr<source> source )
    : mSource(source), mSampleRate(0), mSampleCount(0),
      mChannelCount(0), mBuffer(NULL), mBufferSize(0),
      mBufferFilledSize(0), mBufferOffset(0)
{
    assert(mSource.get());
    
    // Проверяем заголовок RIFF.
    const u32 RIFF = 'FFIR',
              WAVE = 'EVAW';
    u32 chunkId;
    mSource->read(reinterpret_cast<u8 *>(&chunkId), sizeof(u32), 1);
    if (chunkId != RIFF)
        throw std::runtime_error("File is not a valid RIFF container.");
    mSource->skip(4);
    mSource->read(reinterpret_cast<u8 *>(&chunkId), sizeof(u32), 1);
    if (chunkId != WAVE)
        throw std::runtime_error("File is not a valid WAVE container.");
    
    // Читаем настройки.
    const u32 FMT = ' tmf';
    mSource->read(reinterpret_cast<u8 *>(&chunkId), sizeof(u32), 1);
    if (chunkId != FMT)
        throw std::runtime_error("Missing 'fmt ' section after RIFF header.");
    u32 chunkSize;
    mSource->read(reinterpret_cast<u8 *>(&chunkSize), sizeof(u32), 1);
    WAVEFORMATEX format;
    mSource->read(reinterpret_cast<u8 *>(&format), sizeof(WAVEFORMATEX), 1);
    
    // Проверяем правильность формата.
    if (format.wFormatTag != WAVE_FORMAT_PCM)
        throw std::runtime_error("Compressed wave files are not supported.");
    if (format.wBitsPerSample != 16)
        throw std::runtime_error("Unsupported sample format (expected 16 bits).");
    mSampleRate = format.nSamplesPerSec;
    mChannelCount = format.nChannels;
    assert(0 != mChannelCount);
    if (mChannelCount > 2)
        throw std::runtime_error("Expected mono or stereo stream.");
    
    // Перематываем на начало следующего чанка и ищем секцию 'data'.
    const u32 DATA = 'atad';
    mSource->skip(chunkSize - sizeof(WAVEFORMATEX));
    for (;;)
    {
        if (mSource->read(reinterpret_cast<u8 *>(&chunkId), sizeof(u32), 1) != 1)
            throw std::runtime_error("EOF while looking for 'data' section.");
        
        mSource->read(reinterpret_cast<u8 *>(&chunkSize), sizeof(u32), 1);
        if (chunkId == DATA)
            break;
        mSource->skip(chunkSize);
    }
    
    // Секция найдена, в chunkSize ее размер.
    mSampleCount = chunkSize / (sizeof(s16) * mChannelCount);

    // У двухканального файла должно быть четное количество семплов!
    if (mChannelCount == 2)
        mSampleCount = (mSampleCount / 2) * 2;

    mDataOffset = mSource->tell();

    // Буфер декодирования.
    const u32 BUFFER_SIZE = mSampleRate * DECODE_BUFFER_SIZE_IN_MSEC / 1000;
    mBufferSize = (mSampleCount < BUFFER_SIZE ? mSampleCount : BUFFER_SIZE) * mChannelCount;
    mBuffer.reset(new s16[mBufferSize + 1]);
    mBuffer[mBufferSize] = MAGIC; // Значение для проверки выхода за границы.
}

// Декодирование потока во внутренний буфер. Возвращает количество декодированных семплов.
u32 wav_decoder::unpack_to_buffer()
{
    return mSource->read(reinterpret_cast<u8 *>(mBuffer.get()), sizeof(s16), mBufferSize / mChannelCount);
}

// Копирует в левый и правый каналы count декодированных семплов.
u32 wav_decoder::unpack( f32 * left, f32 * right, u32 count )
{
    s16 * ptr = mBuffer.get() + mBufferOffset;
    
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

// Установка курсора начала декодирования на заданный семпл.
void wav_decoder::seek( u32 sample )
{
    assert(sample < mSampleCount);
    mSource->seek(mDataOffset + sample * mChannelCount * sizeof(s16));

    // Сбрасываем указатель на текущий семпл
    mBufferOffset = 0;
    mBufferFilledSize = 0;
}
