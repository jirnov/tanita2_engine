/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef __SOUND_SOUND_H__
#define __SOUND_SOUND_H__

#include "resource/resourceId.h"
#include "base/types.h"
#include "graphics/graphics.h"
#include <tanita2/application/application.h>

// Forward declaration
void exportSound( void );

namespace
{

class resource_source : public flamenco::source
{
public:
    resource_source(const ResourceId & path)
        : mOffset(0), mSize(0)
    {
        ResourceManagerSingleton fm;
        TRY(mResource = fm->loadResource(path));
        mBuffer = reinterpret_cast<u8 *>(mResource->getStream(RESOURCE_STREAM_DEFAULT));
        mSize = mResource->getDataSize();
    }

    u32 read( u8 * destination, u32 size, u32 count )
    {
        assert(NULL != destination);

        u32 bytesRead = std::min(size * count, mSize - mOffset) / size * size;
        if (bytesRead)
        {
            memcpy(destination, mBuffer + mOffset, bytesRead);
            skip(bytesRead);
        }
        return bytesRead / size;
    }

    void skip( s32 offset )
    {
        seek(mOffset + offset);
    }

    void seek( u32 offset )
    {
        mOffset = offset;
        if (mOffset > mSize)
            mOffset = mSize;
    }

    u32 tell() const
    {
        return mOffset;
    }

    bool eof() const
    {
        return mOffset == mSize;
    }

private:
    ResourceRef mResource;
    u8 * mBuffer;
    u32 mSize;
    u32 mOffset;
};

}

//! ������������ ���� �������� �������.
namespace sound
{

//! ������� ����� ��� ������.
/*
 ����� ����� ����� ������ ��������� � ������������, � ����� �����
 ������� ��� �������/��������� ������������.

 ������ ���� ����� ���� ������� �� ����� (������������ ������ ��������
 � �������� ��������), ������� ����� ������ �� ��� ��������� � ������.
 */
class Sound : public utility::refcountable
{
public:
    using utility::refcountable::refcount;

    // ��������� �� ResourceId.
    inline bool isEqualTo( const ResourceId & path )
    {
        return mResourceId.isEqualTo(path);
    }
    inline bool isLessThen( const ResourceId & path )
    {
        return mResourceId.isLessThen(path);
    }

private:

    enum
    {
        SOUND_FLAG_NONE = 0x00,
        /*
          ���� ���� ���� ����������, ���� �� ����� ����������, ���� ����� ��
          ������� ��� � ������ ������������� ������ ��� ������� �����.
         */
        SOUND_FLAG_NON_EXPIRABLE = 0x02,
        /*
          ���� ���� ���� ����������, ��������� ����� �� ����� �� ������
          �� �������� ��������� � ������ �������.
         */
        SOUND_FLAG_NON_POSITIONABLE = 0x04
    };

public:
    //! �������� ����� �� �������
    template <typename decoderT>
    static Sound * create_from_resource( const ResourceId & path )
    {
        return new Sound(path,
            flamenco::stream<decoderT>::create(std::auto_ptr<flamenco::source>(new resource_source(path)))
        );
    }

    //! �������� ����� �� �����
    template <typename decoderT>
    static Sound * create_from_file( const ResourceId & path )
    {
        return new Sound(path,
            flamenco::stream<decoderT>::create(std::auto_ptr<flamenco::source>(new flamenco::file_source(path.getFilePath())))
        );
    }

    //! ������ ������������ �����.
    /**
     ������ ������������ � ���������� ������� (��� �������, ����
     ���� ����������). ������ �� ������, ���� ���� ��� ������.
     */
    void doPlay( void )
    {
        if (mVolumePanEffect->connected())
            return;

        mIsEnqueued = true;
        mPreviosLoopCount = mLoopCount;
        flamenco::mixer & mixer = flamenco::mixer::singleton();
        mixer.attach(mVolumePanEffect);
    }

    //! ��������� ������������ ����� (�����).
    /**
     ������������� ���������������, �� ������� ������� ������������.
     ������ �� ������, ���� ���� ��� ����������.
     */
    void doStop( void )
    {
        if (!mVolumePanEffect->connected())
            return;
        flamenco::mixer & mixer = flamenco::mixer::singleton();
        mixer.detach(mVolumePanEffect);
    }

    //! ������������ ���� �� ������ � ������������� ���������������.
    void doRewind( void )
    {
        doStop();
        mSoundStream->seek(0);
    }

    //! ��������� ������������� ��������� ����� �� �����.
    const D3DXVECTOR2 & getPosition() const
    {
        return mPosition;
    }

    //! ��������� ������������� ��������� ����� �� �����.
    void setPosition( const D3DXVECTOR2 & position )
    {
        mPosition = position;
    }

    //! ��������� ������ ������.
    U32 getGroupId( void ) const
    {
        return mGroupId;
    }

    //! ��������� ������ ������.
    void setGroupId( U32 groupId )
    {
        mGroupId = groupId;
    }

    //! ��������� ��������� �����.
    /** ��������� ��������� �������� �� 0 (������) �� 100 (������ ���������). */
    S32 getVolume( void ) const
    {
        return mVolume * 100;
    }

    //! ��������� ��������� �����.
    /** ��������� ��������� �������� �� 0 (������) �� 100 (������ ���������). */
    void setVolume( S32 volume )
    {
        mVolume = volume / 100.0;
    }

    //! ��������� �������� ������������ �����.
    F32 getPitch( void ) const
    {
        return mPitchEffect->pitch_value();
    }

    //! ��������� �������� ������������ �����.
    void setPitch( F32 pitch )
    {
        mPitchEffect->pitch_value.set(pitch);
    }

    //! ��������� ������� �������.
    /**
     ������ ��������� �������� �� -100 (������ ����� �����)
     �� 100 (������ ������). 0 - �� ������.
     */
    S32 getPanning( void ) const
    {
        return mPan * 100;
    }

    //! ��������� ������� �������.
    /**
     ������ ��������� �������� �� -100 (������ ����� �����)
     �� 100 (������ ������). 0 - �� ������.
     */
    void setPanning( S32 pan )
    {
        mPan = pan / 100.0;
    }

    //! �������� �� ��, ��� ���� �������������.
    bool isPlaying( void ) const
    {
        return mVolumePanEffect->connected() && mSoundStream->playing();
    }

    //! �������� �� ���������� ����� �����.
    /** �������� � ��� ����������� ������. */
    bool isLoopOver( void ) const
    {
        return mLoopCount != mPreviosLoopCount;
    }

    bool getIsLooping() const
    {
        return mSoundStream->looping();
    }

    void setIsLooping( bool value )
    {
        mSoundStream->looping.set(value);
    }

#define DECLARE_BIT_FLAG(flagName, flagMacro) \
    bool getIs##flagName() const { return getSingleFlag(flagMacro); } \
    void setIs##flagName( bool value ) { setSingleFlag(flagMacro, value); }

    DECLARE_BIT_FLAG(NonExpirable,    SOUND_FLAG_NON_EXPIRABLE);
    DECLARE_BIT_FLAG(NonPositionable, SOUND_FLAG_NON_POSITIONABLE);

#undef DECLARE_BIT_FLAG

    //! ���������� ����� � ������ ������������ � ���� ����� ������.
    /**
     �����, �� ����������� � ������ (� ������� �� ����� ����
     IsNonExpirable), ����� �����������. ��� ����������� ����������
     ��� ����� ������������� � ���� �� �����.
     */
    void addToQueue();

    // ���������� ������
    void onUpdate();

    // ��� ��� ������������� ����� � �����.
    U32 getPlayPosition() const
    {
        return mSoundStream->position() * flamenco::LATENCY_MSEC / 440;
    }

    // ����������
    ~Sound( void )
    {
        doStop();
    }

private:
    Sound( const ResourceId & path, const flamenco::reference<flamenco::sound_stream_base> & stream )
        : mResourceId(path),
          mSoundStream(stream),
          mVolume(0.0), mPan(0.0),
          mVolumeCoefficient(1.0),
          mPitchEffect(flamenco::pitch::create(mSoundStream)),
          mVolumePanEffect(flamenco::volume_pan::create(mPitchEffect, 0.0f, 0.0f)),
          mLoopCount(0), mPreviosLoopCount(0),
          mGroupId(0),
          mIsEnqueued(false),
          mSoundFlags(0)
    {
    }

    // �������� �������� �����
    flamenco::reference<flamenco::sound_stream_base> mSoundStream;

    // ������ ��� ��������� pitch
    flamenco::reference<flamenco::pitch> mPitchEffect;

    // ������ ��� ��������� ��������� � �������
    flamenco::reference<flamenco::volume_pan> mVolumePanEffect;

    // ��������� �������� �����
    bool getSingleFlag( U32 flag ) const
    {
        return mSoundFlags & flag;
    }

    // ��������� �������� �����
    void setSingleFlag( U32 flag, bool value )
    {
        value ? mSoundFlags |= flag : mSoundFlags &= ~flag;
    }

    // ���������� ������� �������������
    void updateTransformationMatrix( void )
    {
        D3DXMATRIX tmpTransformation;
        D3DXMatrixTranslation(&tmpTransformation,     mPosition.x,  mPosition.y, 0.0f);

        // Multiplying local matrix by parent matrix
        graphics::Graphics gfx;
        D3DXMatrixMultiply(&mTransformationMatrix, &tmpTransformation,
                           &gfx->getTransformationMatrix());
    }

    // ������� �����
    D3DXVECTOR2 mPosition;

    // ������
    u32 mGroupId;

    // ��������� � ������
    f32 mVolume, mPan;

    // ����������� ����������/���������� ��������� � �����������
    // �� �������� � ��������� ���������
    f32 mVolumeCoefficient;

    // ���������� ������
    u32 mLoopCount, mPreviosLoopCount;

    // ���� �������� � ������� ������������
    bool mIsEnqueued;

    // ������� �������������.
    mat4x4 mTransformationMatrix;

    // ������������� ������� (��������� ��� gc)
    ResourceId mResourceId;

    // ������� ����� ��������� ������
    U32 mSoundFlags;

    friend void exportSound( void );
};

} // namespace sound

//! ������ �� ���� (��� ������������� ��� �������� �������).
typedef utility::reference<sound::Sound> SoundRef;

#endif // __SOUND_SOUND_H__
