/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 ������� ogg vorbis ������
 */
#pragma once
#ifndef _FLAMENCO_INPUT_OGG_H_
#define _FLAMENCO_INPUT_OGG_H_

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace flamenco
{

// ������� ogg vorbis ������.
class ogg_decoder : noncopyable
{
public:
    ogg_decoder( std::auto_ptr<source> source );
    
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
	// ������������� �� vorbis ������ mBufferSize ������� �� ���������� ������.
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

    // ������ ��� ������ � ������� �������.
    auto_array<f32> mBufferL, mBufferR;
    // ������ ������ ������ � �������.
    u32 mBufferSize;

	// ������� ���������� ������� � ������ ������.
	u32 mBufferFilledSize;
	// ������� ����� � ������.
	u32 mBufferOffset;
    
    // ������� ���������� �����
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
    
    // ���������� � ������
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
