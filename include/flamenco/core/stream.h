/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 �������� �������� ������ �� ����� ��� ����� ��������� ������.
 */
#pragma once
#ifndef _FLAMENCO_CORE_STREAM_H_
#define _FLAMENCO_CORE_STREAM_H_

namespace flamenco
{

// ������� ����� ��� �������� �������.
// ����� ��� ����, ����� � ������� ������ ����� ����� ���� ����������
// �� ������ ����������.
class sound_stream_base : public pin
{
public:
    // ���� ������������� �����.
    atomic<bool> looping;

    // ������������� �� ���� (������������ � false, ����� ������������� ���� ������� � �����).
    atomic_readonly<bool, sound_stream_base> playing;

    // ���������� ������, ������� ����� ��������� ����. ���� ���� ����������, ������� ����� 0.
    atomic_readonly<u32, sound_stream_base> loop_count;

    // ������� ������� ������ ����.
    virtual f32 time_total() const = 0;

    // ������� ������� ������ � ������ �����.
    virtual f32 time_elapsed() const = 0;

    // ��������� ������� ������ �� ������������ ����� � ������ ���������� �������
    virtual void seek( u32 sample ) = 0;

    // ����� �������� �������������� ������ � ������ ���������� �������.
    virtual u32 tell() const = 0;

    // ����� ��������� ������ � ������� � ������ ���������� �������.
    virtual u32 size() const = 0;

protected:
    sound_stream_base()
        : looping(false), playing(true), loop_count(0), mPosition(0)
        {}

    // ��������� ������. ������� �����, �.�. ���������� ����� ��������� set().
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

    // ����� �������� �������������� ������ (� ��������� �� LATENCY).
    atomic_readonly<u32, sound_stream_base> mPosition;

private:
    // ������������� ���� playing �� ����� ������������� ������� � �������.
    inline void on_attach(bool connected)
    {
        set_playing(connected && (tell() < size() || looping()));
    }
};

/*
 ��� �������� ������ ��������� ������ ����������:
 struct decoder
 {
     // �����������.
     decoder( std::auto_ptr<source> source );

     // �������� � ����� � ������ ������ count �������������� �������.
     // ���������� ���������� ������������� �������, ��� ����� ���� ������ count,
     // ���� ����� ����������.
     u32 unpack( f32 * left, f32 * right, u32 count );

     // ��������� ������� ������ ������������� �� �������� �����.
     void seek( u32 sample );

     // ����� ������ � �������. ������� �������������� ��� ������ �������.
     u32 length() const;

     // ���������� �������.
     u32 channels() const;

     // ������� ��������� ������ (��� �������������� ������� �������).
     u32 frequency() const;
 }
 */

// �������� �������� ������ �� ������ ��������� ��������.
template <class decoderT>
class stream : public sound_stream_base
{
public:
    // �������� ������.
    static reference<stream> create( std::auto_ptr<source> source );

private:
    stream( std::auto_ptr<source> source )
        : mDecoder(source), mBufferL(NULL), mBufferR(NULL)
    {
        mBufferSizeInSamples = static_cast<u32>(CHANNEL_BUFFER_SIZE_IN_SAMPLES * static_cast<f32>(mDecoder.frequency()) / FREQUENCY);

        // ������� ������ ��� ������������� �������
        if (!is_standard_frequency(mDecoder.frequency()))
        {
            mBufferL.reset(new f32[mBufferSizeInSamples]);
            mBufferR.reset(new f32[mBufferSizeInSamples]);
        }
    }

    // ��������� ������ ������� ��������� �������, ����������� �� ��������.
    // ���� ������� ������ �������� �� ��������� � �������� ����������,
    // ��������� ��������������.
    void process( f32 * left, f32 * right );

    // ������� ������� ������ ����
    inline f32 time_total() const
    {
        return mDecoder.length() / static_cast<f32>(mDecoder.frequency());
    }

    // ������� ������� ������ � ������ �����
    inline f32 time_elapsed() const
    {
        return mPosition() / static_cast<f32>(mDecoder.frequency());
    }

    // ����� ������ ������ � �������.
    inline u32 size() const
    {
        return mDecoder.length() / mDecoder.channels();
    }

    // ��������� ������� ������ �� ������������ �����.
    inline void seek( u32 sample )
    {
        set_position(sample * mDecoder.channels());
        mDecoder.seek(sample * mDecoder.channels());
    }

    // ��������� �������� ������.
    inline u32 tell() const
    {
        return mPosition() / mDecoder.channels();
    }

    // ��������� �� �������������� �������
    inline bool is_standard_frequency( u32 freq ) const
    {
        return freq == FREQUENCY;
    }

    // �������������� �������
    void convert_frequency(f32 * dst, u32 dst_size, const f32 * src, u32 src_size) const
    {
        const f32 rate = static_cast<f32>(src_size) / dst_size;
        for (f32 pos = 0.0, *ptr = dst; pos < src_size; pos += rate)
            *ptr++ = src[static_cast<u32>(pos)];
    }

    // ���������� ������ ��� ������������� ������ (�� 44100).
    auto_array<f32> mBufferL, mBufferR;

    // ������ ���������� ������� � �������.
    u32 mBufferSizeInSamples;

    // �������.
    decoderT mDecoder;
};


// ����������.

template <class decoderT>
reference<stream<decoderT> > stream<decoderT>::create( std::auto_ptr<source> source )
{
    return reference<stream<decoderT> >(new stream<decoderT>(source));
}

template <class decoderT>
void stream<decoderT>::process( f32 * left, f32 * right )
{
    bool looping = this->looping();

    // ���� ������� �����������, �� ���������� ���� �������� � left � right.
    f32 *bufferL = (is_standard_frequency(mDecoder.frequency())) ? left : mBufferL.get(),
        *bufferR = (is_standard_frequency(mDecoder.frequency())) ? right : mBufferR.get();

    // ��������� ������ �������. ���� ����������� ������, ��� �����, ������ ����� ����������.
    u32 count = mDecoder.unpack(bufferL, bufferR, mBufferSizeInSamples);
    if (count < mBufferSizeInSamples)
    {
        if (looping)
        {
            // ��������� �����, ����� ���� ����� � �����.
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
        else // ���� ������� �� �����, ���������� �����.
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

    // ������������� �������
    if (!is_standard_frequency(mDecoder.frequency()))
    {
        convert_frequency(left, CHANNEL_BUFFER_SIZE_IN_SAMPLES, bufferL, mBufferSizeInSamples);
        convert_frequency(right, CHANNEL_BUFFER_SIZE_IN_SAMPLES, bufferR, mBufferSizeInSamples);
    }
}

} // namespace flamenco

#endif // _FLAMENCO_CORE_STREAM_H_
