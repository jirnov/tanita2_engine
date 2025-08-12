/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "base/types.h"
#include "base/assert.h"

#include "sound/manager.h"
#include "sound/sound.h"

#include <tanita2/application/application.h>

using namespace sound;

// Второй поток звуковой системы.
U32 WINAPI sound::SoundThreadProc( void * ptrToManager )
{
    Manager * manager = static_cast<Manager *>(ptrToManager);

    LPDIRECTSOUND8 directSound;

    // Создаем интерфейс DirectSound.
    check_directx(DirectSoundCreate8(&GUID_NULL, &directSound, NULL));
    check_directx(directSound->SetCooperativeLevel(application::Application::window(), DSSCL_PRIORITY));

    // Получаем первичный буфер.
    LPDIRECTSOUNDBUFFER primaryBuffer = NULL;
    DSBUFFERDESC descr;
    ZeroMemory(&descr, sizeof(DSBUFFERDESC));
    descr.dwSize = sizeof(DSBUFFERDESC);
    descr.dwFlags = DSBCAPS_PRIMARYBUFFER;
    descr.lpwfxFormat = NULL;
    check_directx(directSound->CreateSoundBuffer(&descr, &primaryBuffer, NULL));

    // Изменяем формат первичного буфера.
    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.cbSize          = sizeof(WAVEFORMATEX);
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = 2;
    wfx.nSamplesPerSec  = flamenco::FREQUENCY;
    wfx.wBitsPerSample  = 16;
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = (u32)wfx.nSamplesPerSec * wfx.nBlockAlign;
    check_directx(primaryBuffer->SetFormat(&wfx));
    primaryBuffer->Release();


    // Формат буфера микшера.
    const u32 MIXER_BUFFER_SIZE_IN_BYTES = flamenco::MIXER_BUFFER_SIZE_IN_SAMPLES * sizeof(s16);
    // Размер звукового буфера должен быть больше 100 ms, иначе GetCurrentPosition()
    // будет выдавать неправильные данные.
    const u32 SOUND_BUFFER_SIZE_IN_BYTES = 10 * MIXER_BUFFER_SIZE_IN_BYTES;
    DSBUFFERDESC desc;
    ZeroMemory(&desc, sizeof(DSBUFFERDESC));
    desc.dwSize          = sizeof(DSBUFFERDESC);
    desc.dwFlags         = DSBCAPS_LOCSOFTWARE | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS;
    desc.dwBufferBytes   = SOUND_BUFFER_SIZE_IN_BYTES;
    desc.guid3DAlgorithm = DS3DALG_DEFAULT;
    desc.lpwfxFormat     = &wfx;

    // Создаем буфер.
    LPDIRECTSOUNDBUFFER soundBuffer = NULL;
    check_directx(directSound->CreateSoundBuffer(&desc, &soundBuffer, NULL));

    // Инициализация flamenco.
    flamenco::mixer & mixer = flamenco::mixer::singleton();

    // Заполнение звукового буфера.
    s16 * bufferPtr;
    u32 bufferSize;
    check_directx(soundBuffer->Lock(0, 0, reinterpret_cast<void **>(&bufferPtr), &bufferSize,
        NULL, NULL, DSBLOCK_ENTIREBUFFER));
    // Заполняем обе половинки буфера.
    mixer.mix(bufferPtr);
    mixer.mix(bufferPtr + flamenco::MIXER_BUFFER_SIZE_IN_SAMPLES);
    check_directx(soundBuffer->Unlock(bufferPtr, bufferSize, NULL, 0));

    // Устанавливаем максимальную громкость звукового буфера.
    soundBuffer->SetVolume(DSBVOLUME_MAX);
    // Проигрываем звук и дописываем данные по ходу.
    soundBuffer->Play(0, 0, DSBPLAY_LOOPING);
    u32 writeOffset = MIXER_BUFFER_SIZE_IN_BYTES * 2;
    
    while (!manager->mDoTerminateThread)
    {
        u32 cursorPos;
        soundBuffer->GetCurrentPosition(&cursorPos, NULL);

        // Определяем, нужно ли дописать очередную порцию данных.
        u32 offset = (SOUND_BUFFER_SIZE_IN_BYTES + writeOffset - cursorPos) % SOUND_BUFFER_SIZE_IN_BYTES;
        if (offset > MIXER_BUFFER_SIZE_IN_BYTES)
        {
            check_directx(soundBuffer->Lock(writeOffset, MIXER_BUFFER_SIZE_IN_BYTES,
                reinterpret_cast<void **>(&bufferPtr), &bufferSize,
                NULL, NULL, 0));

            if (manager->mDoPauseThread)
                memset(bufferPtr, 0, MIXER_BUFFER_SIZE_IN_BYTES);
            else
                mixer.mix(bufferPtr);

            check_directx(soundBuffer->Unlock(bufferPtr, bufferSize, NULL, 0));
            writeOffset = (writeOffset + MIXER_BUFFER_SIZE_IN_BYTES) % SOUND_BUFFER_SIZE_IN_BYTES;
        }
        // Не нужно опрашивать GetCurrentPosition() слишком часто.
        Sleep(flamenco::LATENCY_MSEC >> 1);
    }

    // Освобождение ресурсов.
    soundBuffer->Release();
    directSound->Release();

    return 0;
}

// Инициализация звуковой системы.
Manager::Manager()
    : mSoundContainer("Sound manager"),
      mThread(NULL),
      mDoPauseThread(false),
      mDoTerminateThread(false)
{
    // Настраиваем уровни громкости.
    const S32 MAX_VOLUME = 100;
    for (int i = 0; i < config::sound::MAX_GROUP_COUNT; ++i)
        mGroupVolume[i] = 0;
    
    mThread = CreateThread(NULL, 0, SoundThreadProc, (void *)this, 0, NULL);
    check_win32(mThread);
}

// Выгрузка звуков и ресурсов.
Manager::~Manager()
{
    // Ждем завершения потока.
    if (NULL != mThread)
    {
        mDoTerminateThread = TRUE;
        if (WAIT_TIMEOUT == WaitForSingleObject(mThread, config::sync::MAX_WAIT_TIME))
        {
            logger()->error() << "Sound manager thread termination timed out.";
            TerminateThread(mThread, 0);
        }
    }
    
    // Удаляем все звуки.
    mSoundContainer.clear();
}

void Manager::doGarbageCollection()
{
    mSoundContainer.release_unused();
}

// Обновление состояния звуков, чистка кеша.
void Manager::doUpdate( U32 dt )
{
    static U32 timer = 0;
    
    // Удаляем ненужные звуки.
    timer += dt;
    if (timer > config::sound::CLEANUP_PERIOD)
    {
        timer = 0;
        doGarbageCollection();
    }
    // Включаем поток
    mDoPauseThread = false;

    // Updating sounds
    mSoundContainer.for_each(&Sound::onUpdate);
}

void Manager::doSkipUpdate()
{
    // Отключаем обновление звуков
    mDoPauseThread = true;
}

// Загрузка звука.
SoundRef Manager::loadSound( const ResourceId & path )
{
    // Создаем звук соответствующего типа.
    Sound * sound = NULL;

    switch (path.getType())
    {
    case RESOURCE_TYPE_WAV:
        sound = Sound::create_from_resource<flamenco::wav_decoder>(path);
        break;
    case RESOURCE_TYPE_OGG:
        // Vorbis-звук, всегда поточный.
        sound = Sound::create_from_file<flamenco::ogg_decoder>(path);
        break;
    }

    // Добавляем звук в кеш.
    ASSERT(sound);
    return SoundRef(mSoundContainer.insert(mSoundContainer.lower_bound(path), utility::reference<Sound>(sound)));
}

// Получить общую громкость звуков данной группы.
U32 Manager::getGroupMasterVolume( U32 groupId ) const
{
    if (groupId >= config::sound::MAX_GROUP_COUNT)
        throw Exception("Sound group id exceeds maximum group count.");
    return mGroupVolume[groupId];
}

// Задать общую громкость звуков данной группы.
void Manager::setGroupMasterVolume( U32 groupId, U32 volume )
{
    if (groupId >= config::sound::MAX_GROUP_COUNT)
        throw Exception("Sound group id exceeds maximum group count.");
    mGroupVolume[groupId] = volume;
}

template void base::resource_container<Sound, true>::clear();

// Вставка элемента в контейнер.
template base::resource_container<Sound, true>::value_type
    base::resource_container<Sound, true>::insert( 
        base::resource_container<Sound, true>::const_iterator & where,
        base::resource_container<Sound, true>::value_type element );
