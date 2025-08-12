#include <precompiled.h>
#include "SoundSequenceBase_.h"
#include "LargeAnimatedSoundSequence_.h"
#include "sound/manager.h"

using namespace ingame;

// Constructor
SoundSequenceBase::SoundSequenceBase( FrameSounds & asounds )
    : old_position(0)
{
    for (FrameSoundsIter i = asounds.begin(); asounds.end() != i; ++i)
    {
        bool found = false;
        for (FrameSoundsExIter j = attached_sounds.begin(); attached_sounds.end() != j; ++j)
            if (j->first == i->first)
            {
                j->second.push_back(i->second);
                found = true;
                break;
            }
        if (found) continue;
        attached_sounds.push_back(std::pair<int, FrameSoundArray>(i->first, FrameSoundArray()));
        attached_sounds.back().second.push_back(i->second);
    }
}

// Stop sequence
void SoundSequenceBase::stop()
{
    for (FrameSoundsExIter j = attached_sounds.begin(); attached_sounds.end() != j; ++j)
        for (FrameSoundArrayIter i = j->second.begin(); j->second.end() != i; ++i)
            if (!(*i)->getIsNonExpirable())
                (*i)->doRewind();
}

// Updating
void SoundSequenceBase::update_sound_queue( float /* dt */, int previous_frame,
                                            int current_frame )
{
    if (previous_frame == current_frame) return;
    
    SoundManager sm;
    if (previous_frame < current_frame)
    {
        for (FrameSoundsExIter i = attached_sounds.begin(); attached_sounds.end() != i; ++i)
            if (previous_frame <= i->first && i->first < current_frame)
            {
                SoundRef & s = i->second[rand() % i->second.size()];
                s->doRewind();
                s->doPlay();
                current_sound = s;
                old_position = 0;
            }
    }
    else
        for (FrameSoundsExIter i = attached_sounds.begin(); attached_sounds.end() != i; ++i)
            if (previous_frame <= i->first || i->first < current_frame)
            {
                SoundRef & s = i->second[rand() % i->second.size()];
                s->doRewind();
                s->doPlay();
                current_sound = s;
                old_position = 0;
            }
}

// Calculate dt from current sound
float SoundSequenceBase::calculate_dt( float dt )
{
    return dt;
}
