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
    //! ������������� �������� �������.
    Manager();
    //! ������������ ��������� ��������.
    ~Manager();
    
    void doGarbageCollection();
    
    //! �������� �����.
    SoundRef loadSound( const ResourceId & path );
    
    //! �������� ����� ��������� ������ ������ ������.
    U32 getGroupMasterVolume( U32 groupId ) const;
    //! ������ ����� ��������� ������ ������ ������.
    void setGroupMasterVolume( U32 groupId, U32 volume );    
    
    // ���������� ��������� ������, ������ ����.
    void doUpdate( U32 dt );

	// �������� ��� �����
	void doSkipUpdate();
    
private:
    // ��� ��� ���������� ������.
    typedef base::resource_container<class Sound, true> SoundContainer;
    // ��������� ��� ������.
    SoundContainer mSoundContainer;

    // ������ ��������� ��� �����.
    S32 mGroupVolume[config::sound::MAX_GROUP_COUNT];
    
    // Handle ������� ������ (����������� �� ��������� ��������).
    HANDLE mThread;
    // ���� ��� ���������� ������� ������
    base::sync::interlocked_bool mDoTerminateThread;
    // ���� ��� ���������� �������������� ������� ������
    base::sync::interlocked_bool mDoPauseThread;    

    friend U32 WINAPI SoundThreadProc( void * ptrToManager );
};

} // end of namespace sound

//! ���������� ���������� ��� ��������� ���������.
typedef utility::singleton<sound::Manager> SoundManager;

#endif // end of __SOUND_MANAGER_H__
