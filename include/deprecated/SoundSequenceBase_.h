#pragma once
#include "sound/sound.h"

// Namespace for game object type declarations
namespace ingame
{

//! Base class for sequences with sound
class SoundSequenceBase
{
public:
    //! Type for sounds attached to frame indices
    typedef std::vector<std::pair<int, SoundRef> > FrameSounds;
    typedef FrameSounds::iterator FrameSoundsIter;
    
    //! Constructor
    SoundSequenceBase( FrameSounds & attached_sounds );
    
    //! Destructor
    virtual inline ~SoundSequenceBase() = 0 {}
    
    //! Updating
    void update_sound_queue( float dt, int previous_frame,
                             int current_frame );

    // Stop sequence
    void stop();
    
protected:
    typedef std::vector<std::pair<int, std::vector<SoundRef> > > FrameSoundsEx;
    typedef FrameSoundsEx::iterator FrameSoundsExIter;
    typedef std::vector<SoundRef> FrameSoundArray;
    typedef std::vector<SoundRef>::iterator FrameSoundArrayIter;
    
    // Current sound for dt calculation
    SoundRef current_sound;
    
    // Calculate dt from current sound
    float calculate_dt( float dt );
    DWORD old_position;
    
    //! Sounds
    FrameSoundsEx attached_sounds;
};

}