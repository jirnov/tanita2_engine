/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 ���������� ��������� ����� �� wav-�����.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

namespace 
{

// ������ ������ �� ������.
size_t read_func( void * ptr, size_t size, size_t count, void * datasource )
{
    source * input = reinterpret_cast<source *>(datasource);
    return static_cast<size_t>(input->read(static_cast<u8 *>(ptr), static_cast<u32>(size),
                                           static_cast<u32>(count)));
}

// ����������� �� ������.
int seek_func( void * datasource, ogg_int64_t offset, int whence )
{
    source * input = reinterpret_cast<source *>(datasource);
    switch(whence)
    {
    case SEEK_SET:
        input->seek(static_cast<u32>(offset));
        break;
    case SEEK_CUR:
        input->skip(static_cast<s32>(offset));
        break;
    case SEEK_END:
        input->seek(input->size());
        break;                
    }
    return 0;
}

// ��������� �������� � ������.
long tell_func( void * datasource )
{
    return reinterpret_cast<source *>(datasource)->tell();
}

// �������� ������ - �� ���������.
int close_func( void * )
{
    return 0;
}

// ��������� � ����������� �� ������� ��� ������ � �������.
ov_callbacks gCallbacks = {
    read_func,
    seek_func, 
    close_func,
    tell_func
};

} // namespace flamenco


// �����������.
ogg_decoder::ogg_decoder( std::auto_ptr<source> source )
    : mSource(source), mSampleRate(0), mSampleCount(0),
      mChannelCount(0), mBufferL(NULL), mBufferR(NULL), 
      mBufferSize(0), mBufferFilledSize(0), mBufferOffset(0)
{
    assert(mSource.get());

    // ������� � ��������� vorbis �����.
    mVorbisFile.reset(new OggVorbis_File);
    if (ov_open_callbacks(mSource.get(), mVorbisFile.get(), NULL, -1, gCallbacks) < 0)
        throw std::runtime_error("File is not a valid ogg container.");

    // ���������� � ������ vorbis.
    mVorbisInfo.reset(ov_info(mVorbisFile.get(), -1));

    // ���������� �������.
    mChannelCount = mVorbisInfo->channels;
    assert(mChannelCount != 0);
    if (mChannelCount > 2)
        throw std::runtime_error("Expected mono or stereo stream.");

    // �������.
    mSampleRate = mVorbisInfo->rate;

    // ����� ���������� �������.
    mSampleCount = static_cast<u32>(ov_pcm_total(mVorbisFile.get(), -1));

    // ������ ������.
    const u32 BUFFER_SIZE = mSampleRate * DECODE_BUFFER_SIZE_IN_MSEC / 1000;
    mBufferSize = std::min(BUFFER_SIZE, mSampleCount);
    
    mBufferL.reset(new f32[mBufferSize]);
    if (mChannelCount == 2)
        mBufferR.reset(new f32[mBufferSize]);
}

// ��������� ������� ������ ������������� �� �������� �����.
void ogg_decoder::seek( u32 sample )
{
    if (ov_pcm_seek(mVorbisFile.get(), sample) != 0)
        throw std::runtime_error("Seeking error.");

    // ���������� ��������� �� ������� �����
    mBufferOffset = 0;
    // �������� ������
    mBufferFilledSize = 0;
}

// ������������� �� vorbis ������ count ������� �� ���������� �����.
// ���������� ���������� ����������� �������.
u32 ogg_decoder::unpack_to_buffer()
{
    u32 samplesRead = 0;
    
    while (samplesRead < mBufferSize)
    {
        float ** pcm;
        int result = ov_read_float(mVorbisFile.get(), &pcm, mBufferSize - samplesRead, NULL);
        if (result == 0)
            break;
        else if (result > 0)
        {
            memcpy(mBufferL.get() + samplesRead, pcm[0], result * sizeof(f32));
            if (mChannelCount == 2)
                memcpy(mBufferR.get() + samplesRead, pcm[1], result * sizeof(f32));
            samplesRead += result;
        }
    }
    return samplesRead;
}

// �������� � ����� � ������ ������ count �������������� �������.
u32 ogg_decoder::unpack( f32 * left, f32 * right, u32 count )
{
    u32 sampleCount = 0;
    f32 * bufferLeft = mBufferL.get(),
        * bufferRight = (mChannelCount == 2 ? mBufferR.get() : bufferLeft);
    
    while (sampleCount < count)
    {
        if (mBufferOffset >= mBufferFilledSize)
        {
            // �������� ���������� ��� ������ � �����
            mBufferOffset = 0;
            if (0 == (mBufferFilledSize = unpack_to_buffer()))
                break;
        }
        const u32 SAMPLES_COUNT = std::min(mBufferFilledSize - mBufferOffset, count - sampleCount);
        memcpy(left + sampleCount, bufferLeft + mBufferOffset, SAMPLES_COUNT * sizeof(f32));
        memcpy(right + sampleCount, bufferRight + mBufferOffset, SAMPLES_COUNT * sizeof(f32));
        sampleCount += SAMPLES_COUNT;
        mBufferOffset += SAMPLES_COUNT;
    }
    return sampleCount;
}
