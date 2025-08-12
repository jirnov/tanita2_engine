#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

//#include "sound/sound.h"
#include "resource/resourceId.h"
#include "base/container/resourceContainer.h"
#include "base/sync/interlockedTypes.h"

// Forward declaration
namespace sound
{
    class Sound;
}
typedef utility::reference<sound::Sound> SoundRef;

namespace sound
{

class Manager : boost::noncopyable
{
public:  
    //! Инициализация звуковой системы.
    Manager();
    //! Освобождение системных ресурсов.
    ~Manager();
    
    void doGarbageCollection();
    
    //! Загрузка звука.
    SoundRef loadSound( const ResourceId & path );
    
    //! Получить общую громкость звуков данной группы.
    U32 getGroupMasterVolume( U32 groupId ) const;
    //! Задать общую громкость звуков данной группы.
    void setGroupMasterVolume( U32 groupId, U32 volume );    
    
    // Обновление состояния звуков, чистка кеша.
    void doUpdate( U32 dt );

	// Затыкаем все звуки
	void doSkipUpdate();
    
private:
    // Тип для контейнера звуков.
    typedef base::resource_container<class Sound, true> SoundContainer;
    // Контейнер для звуков.
    SoundContainer mSoundContainer;

    // Уровни громкости для групп.
    S32 mGroupVolume[config::sound::MAX_GROUP_COUNT];
    
    // Handle второго потока (работающего со звуковыми буферами).
    HANDLE mThread;
    // Флаг для прерывания второго потока
    base::sync::interlocked_bool mDoTerminateThread;
    // Флаг для временного останавливания второго потока
    base::sync::interlocked_bool mDoPauseThread;    

    friend U32 WINAPI SoundThreadProc( void * ptrToManager );
};

} // end of namespace sound

//! Объявление синглетона для звукового менеджера.
typedef utility::singleton<sound::Manager> SoundManager;

#endif // end of __SOUND_MANAGER_H__
