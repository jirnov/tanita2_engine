/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 Декодер ogg vorbis файлов
 */
#pragma once
#ifndef _FLAMENCO_INPUT_OGG_H_
#define _FLAMENCO_INPUT_OGG_H_

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace flamenco
{

// Декодер ogg vorbis файлов.
class ogg_decoder : noncopyable
{
public:
    ogg_decoder( std::auto_ptr<source> source );
    
    // Копирует в левый и правый каналы count декодированных семплов.
    // Возвращает количество скопированных семплов, оно может быть меньше count,
    // если поток закончился.
    u32 unpack( f32 * left, f32 * right, u32 count );
    
    // Установка курсора начала декодирования на заданный семпл.
    void seek( u32 sample );

    // Длина потока в семплах. Функция оптимизирована для частых вызовов.
    inline u32 length() const
    {
        return mSampleCount;
    }

    // Количество каналов
    inline u32 channels() const
    {
        return mChannelCount;
    }
    
    // Частота звукового потока (для преобразования частоты потоком).
    inline u32 frequency() const
    {
        return mSampleRate;
    }
    
private:
	// Распаковывает из vorbis потока mBufferSize семплов во внутренние буферы.
	// Возвращает количество прочитанных семплов.
	u32 unpack_to_buffer();

    // Источник данных.
    std::auto_ptr<source> mSource;

    // Частота дискретизации.
    u32 mSampleRate;
    // Длина звука в семплах.
    u32 mSampleCount;
    // Количество каналов (1 или 2).
    u32 mChannelCount;

    // Буферы для левого и правого каналов.
    auto_array<f32> mBufferL, mBufferR;
    // Размер одного буфера в семплах.
    u32 mBufferSize;

	// Текущее количество семплов в каждом буфере.
	u32 mBufferFilledSize;
	// Текущий семпл в буфере.
	u32 mBufferOffset;
    
    // Входной логический поток
    template <typename>
    struct VorbisFileDeleter
    {
        void operator()( OggVorbis_File * f )
        {
            ov_clear(f);
            delete f;
        }
    };
    auto_ptr<OggVorbis_File, VorbisFileDeleter> mVorbisFile;
    
    // Информация о потоке
    template <typename>
    struct VorbisInfoDeleter
    {
        void operator()( vorbis_info * info )
        {
            vorbis_info_clear(info);
        }
    };
    auto_ptr<vorbis_info, VorbisInfoDeleter> mVorbisInfo;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAV_H_
