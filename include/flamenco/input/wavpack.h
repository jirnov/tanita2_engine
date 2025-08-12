/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 �������� ����� �� wv-�����.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_WAVPACK_H_
#define _FLAMENCO_INPUT_WAVPACK_H_

#include <wavpack/wavpack.h>

namespace flamenco
{

// �������� ����� �� wavpack-�����.
class wavpack_decoder : noncopyable
{
public:
    wavpack_decoder( std::auto_ptr<source> source );

    // �������� � ����� � ������ ������ count �������������� �������.
    // ���������� ���������� ������������� �������, ��� ����� ���� ������ count,
    // ���� ����� ����������.
    u32 unpack( f32 * left, f32 * right, u32 count );

    // ��������� ������� ������ ������������� �� �������� �����.
    void seek( u32 sample );

    // ����� ������ � �������. ������� �������������� ��� ������ �������.
    inline u32 length() const
    {
        return mSampleCount;
    }

    // ���������� �������
    inline u32 channels() const
    {
        return mChannelCount;
    }
    
    // ������� ��������� ������ (��� �������������� ������� �������).
    inline u32 frequency() const
    {
        return mSampleRate;
    }

private:
    // ������������� �� wavpack ������ mBufferSize ������� � mBuffer.
    // ���������� ���������� ����������� �������.
    u32 unpack_to_buffer();

    // �������� ������.
    std::auto_ptr<source> mSource;

    // ������� �������������.
    u32 mSampleRate;
    // ����� ����� � �������.
    u32 mSampleCount;
    // ���������� ������� (1 ��� 2).
    u32 mChannelCount;

    // ����� ��� �������������� ������� �� interleaved s32 � separate f32.
    auto_array<s32> mBuffer;
    // ������ ������ � �������.
    u32 mBufferSize;

    // ������� ���������� ������� � ������ (��� ����� ����� ����� ���� ������ mBufferSize).
    u32 mBufferFilledSize;
    // ������� ����� � ������.
    u32 mBufferOffset;

    // Deleter ��� WavpackContext
    template <class T>
    struct context_deleter
    {
        void operator ()( WavpackContext * context )
        {
            WavpackCloseFile(context);
        }
    };
    // �������� ������������� wavpack.
    auto_ptr<WavpackContext, context_deleter> mWavpackFile;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAVPACK_H_
