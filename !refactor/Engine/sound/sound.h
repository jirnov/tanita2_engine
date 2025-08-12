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

//! Пространство имен звуковой системы.
namespace sound
{

//! Базовый класс для звуков.
/*
 Звуки имеют набор флагов состояния и проигрывания, а также набор
 функций для запуска/остановки проигрывания.

 Каждый звук имеет свою позицию на сцене (относительно своего родителя
 в иерархии объектов), которая может влиять на его громкость и баланс.
 */
class Sound : public utility::refcountable
{
public:
    using utility::refcountable::refcount;

    // Сравнение по ResourceId.
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
          Если этот флаг установлен, звук не будет остановлен, если никто не
          добавил его в список проигрываемых звуков для данного кадра.
         */
        SOUND_FLAG_NON_EXPIRABLE = 0x02,
        /*
          Если этот флаг установлен, положение звука на сцене не влияет
          на итоговую громкость и баланс каналов.
         */
        SOUND_FLAG_NON_POSITIONABLE = 0x04
    };

public:
    //! Загрузка звука из ресурса
    template <typename decoderT>
    static Sound * create_from_resource( const ResourceId & path )
    {
        return new Sound(path,
            flamenco::stream<decoderT>::create(std::auto_ptr<flamenco::source>(new resource_source(path)))
        );
    }

    //! Загрузка звука из файла
    template <typename decoderT>
    static Sound * create_from_file( const ResourceId & path )
    {
        return new Sound(path,
            flamenco::stream<decoderT>::create(std::auto_ptr<flamenco::source>(new flamenco::file_source(path.getFilePath())))
        );
    }

    //! Запуск проигрывания звука.
    /**
     Запуск проигрывания с предыдущей позиции (или сначала, если
     звук закончился). Ничего не делает, если звук уже играет.
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

    //! Остановка проигрывания звука (пауза).
    /**
     Останавливает воспроизведение, не изменяя позиции проигрывания.
     Ничего не делает, если звук уже остановлен.
     */
    void doStop( void )
    {
        if (!mVolumePanEffect->connected())
            return;
        flamenco::mixer & mixer = flamenco::mixer::singleton();
        mixer.detach(mVolumePanEffect);
    }

    //! Перематывает звук на начало и останавливает воспроизведение.
    void doRewind( void )
    {
        doStop();
        mSoundStream->seek(0);
    }

    //! Получение относительных координат звука на сцене.
    const D3DXVECTOR2 & getPosition() const
    {
        return mPosition;
    }

    //! Установка относительных координат звука на сцене.
    void setPosition( const D3DXVECTOR2 & position )
    {
        mPosition = position;
    }

    //! Получение номера группы.
    U32 getGroupId( void ) const
    {
        return mGroupId;
    }

    //! Изменение номера группы.
    void setGroupId( U32 groupId )
    {
        mGroupId = groupId;
    }

    //! Получение громкости звука.
    /** Громкость принимает значения от 0 (тишина) до 100 (полная громкость). */
    S32 getVolume( void ) const
    {
        return mVolume * 100;
    }

    //! Изменение громкости звука.
    /** Громкость принимает значения от 0 (тишина) до 100 (полная громкость). */
    void setVolume( S32 volume )
    {
        mVolume = volume / 100.0;
    }

    //! Получение скорости проигрывания звука.
    F32 getPitch( void ) const
    {
        return mPitchEffect->pitch_value();
    }

    //! Изменение скорости проигрывания звука.
    void setPitch( F32 pitch )
    {
        mPitchEffect->pitch_value.set(pitch);
    }

    //! Получение баланса каналов.
    /**
     Баланс принимает значения от -100 (только левый канал)
     до 100 (только правый). 0 - по центру.
     */
    S32 getPanning( void ) const
    {
        return mPan * 100;
    }

    //! Изменение баланса каналов.
    /**
     Баланс принимает значения от -100 (только левый канал)
     до 100 (только правый). 0 - по центру.
     */
    void setPanning( S32 pan )
    {
        mPan = pan / 100.0;
    }

    //! Проверка на то, что звук проигрывается.
    bool isPlaying( void ) const
    {
        return mVolumePanEffect->connected() && mSoundStream->playing();
    }

    //! Проверка на достижение конца звука.
    /** Работает и для зацикленных звуков. */
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

    //! Добавление звука в список используемых в этом кадре звуков.
    /**
     Звуки, не добавленные в список (у которых не стоит флаг
     IsNonExpirable), будут остановлены. При последующем добавлении
     они будут проигрываться с того же места.
     */
    void addToQueue();

    // Обновление флагов
    void onUpdate();

    // Хак для синхронизации видео и звука.
    U32 getPlayPosition() const
    {
        return mSoundStream->position() * flamenco::LATENCY_MSEC / 440;
    }

    // Деструктор
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

    // Основной звуковой поток
    flamenco::reference<flamenco::sound_stream_base> mSoundStream;

    // Эффект для изменения pitch
    flamenco::reference<flamenco::pitch> mPitchEffect;

    // Эффект для изменения громкости и баланса
    flamenco::reference<flamenco::volume_pan> mVolumePanEffect;

    // Получение битового флага
    bool getSingleFlag( U32 flag ) const
    {
        return mSoundFlags & flag;
    }

    // Установка битового флага
    void setSingleFlag( U32 flag, bool value )
    {
        value ? mSoundFlags |= flag : mSoundFlags &= ~flag;
    }

    // Обновление матрицы трансформации
    void updateTransformationMatrix( void )
    {
        D3DXMATRIX tmpTransformation;
        D3DXMatrixTranslation(&tmpTransformation,     mPosition.x,  mPosition.y, 0.0f);

        // Multiplying local matrix by parent matrix
        graphics::Graphics gfx;
        D3DXMatrixMultiply(&mTransformationMatrix, &tmpTransformation,
                           &gfx->getTransformationMatrix());
    }

    // Позиция звука
    D3DXVECTOR2 mPosition;

    // Группа
    u32 mGroupId;

    // Громкость и баланс
    f32 mVolume, mPan;

    // Коэффициент увеличение/уменьшения громкости в зависимости
    // от удаления и групповой громкости
    f32 mVolumeCoefficient;

    // Внутренние данные
    u32 mLoopCount, mPreviosLoopCount;

    // Звук добавлен в очередь проигрывания
    bool mIsEnqueued;

    // Матрица трансформации.
    mat4x4 mTransformationMatrix;

    // Идентификатор ресурса (необходим для gc)
    ResourceId mResourceId;

    // Битовые Флаги звукового потока
    U32 mSoundFlags;

    friend void exportSound( void );
};

} // namespace sound

//! Ссылка на звук (для использования вне звуковой системы).
typedef utility::reference<sound::Sound> SoundRef;

#endif // __SOUND_SOUND_H__
