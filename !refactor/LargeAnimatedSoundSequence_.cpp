#include <precompiled.h>
#include "LargeAnimatedSoundSequence_.h"

using namespace ingame;

// Updating
bool LargeAnimatedSoundSequence::onAddingToQueue( float dt )
{
    int old_frame = mCurrentFrameIndex;
    dt = calculate_dt(dt);
    bool result = LargeAnimatedSequence::onAddingToQueue(dt);

    update_sound_queue(dt, old_frame, mCurrentFrameIndex);
    return result;
}
