/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "config/config.h"

#include "sound/manager.h"
#include "sound/sound.h"

using namespace sound;

// Добавление звука в список используемых в этом кадре звуков.
void Sound::addToQueue()
{
    mIsEnqueued = true;
    
    // Обновляем матрицы трансформации.
    updateTransformationMatrix();
    
    if (!isPlaying())
        return;
    
    f32 panning = mPan;

    mVolumeCoefficient = 1.0;
    
    if (!getSingleFlag(SOUND_FLAG_NON_POSITIONABLE))
    {
        D3DXVECTOR4 screenPoint;
        D3DXVec2Transform(&screenPoint, &D3DXVECTOR2(0, 0), &mTransformationMatrix);
        const F32 dx = screenPoint.x - (config::gfx::SCREEN_WIDTH / 2),
                  dy = screenPoint.y - (config::gfx::SCREEN_HEIGHT / 2);

        const F32 FADE_DISTANCE = config::gfx::SCREEN_WIDTH / 2;
        const F32 OBJECT_DISTANCE = sqrtf(dx * dx + dy * dy);

        if (OBJECT_DISTANCE != 0)
            mVolumeCoefficient *= flamenco::clamp(FADE_DISTANCE/OBJECT_DISTANCE, 0.0f, 1.0f);

        panning += dx / (config::gfx::SCREEN_WIDTH);
    }

    SoundManager sm;
    mVolumeCoefficient = flamenco::clamp(mVolumeCoefficient * static_cast<f32>(sm->getGroupMasterVolume(mGroupId)) / 100.0f, 0.0f, 1.0f);
    mVolumePanEffect->pan.set(flamenco::clamp(panning, -1.0f, 1.0f));
}

// Вызывается в основном потоке для обновления флагов.
void Sound::onUpdate()
{
    mPreviosLoopCount = mLoopCount;
    mLoopCount = mSoundStream->loop_count();
    
    if (isPlaying())
        mVolumePanEffect->volume.set(mVolume * mVolumeCoefficient);

    if (isLoopOver() && !mSoundStream->looping())
        doRewind();

    if (!mIsEnqueued && isPlaying() && !getSingleFlag(SOUND_FLAG_NON_EXPIRABLE))
        doRewind();
        
    mIsEnqueued = false;
}

// Шаблон для использования smart_pointer в boost::python.
namespace boost {
namespace python
{
    template<class T>
    inline T * get_pointer( const utility::reference<T> & p ) { return const_cast<T *>(&*p); }
    
    template<typename T>
    struct pointee<utility::reference<T> >
    {
        typedef T type;
    };
}}

// Bind class to python
void exportSound( void )
{
    using namespace sound;
    using namespace bp;
    class_<Sound, SoundRef, boost::noncopyable>("Sound", no_init)
        .add_property("group", &Sound::getGroupId,
                               &Sound::setGroupId)
        
        .def("render",     &Sound::addToQueue)
        
        .add_property("position", make_function(&Sound::getPosition,
                      return_internal_reference<>()), &Sound::setPosition)
        .add_property("looped",    &Sound::getIsLooping,
                                   &Sound::setIsLooping)
        .add_property("prolonged", &Sound::getIsNonExpirable,
                                   &Sound::setIsNonExpirable)
        .add_property("nonpositionable", &Sound::getIsNonPositionable,
                                         &Sound::setIsNonPositionable)
        
        .add_property("is_playing",  &Sound::isPlaying)
        .add_property("is_over",     &Sound::isLoopOver)
        
        .def("play",       &Sound::doPlay)
        .def("stop",       &Sound::doStop)
        .def("rewind",     &Sound::doRewind)
        
        .add_property("volume",    &Sound::getVolume,
                                   &Sound::setVolume)
        .add_property("pan",       &Sound::getPanning,
                                   &Sound::setPanning)
        .add_property("pitch",     &Sound::getPitch,
                                   &Sound::setPitch)
        ;
}
