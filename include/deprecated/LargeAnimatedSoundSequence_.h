#pragma once
#include "SoundSequenceBase_.h"
#include "LargeAnimatedSequence_.h"

// Namespace for game object type declarations
namespace ingame
{

//! Large animated sequence with sound
class LargeAnimatedSoundSequence: public LargeAnimatedSequence, public SoundSequenceBase
{
public:
    //! Creating sequence for given path, collection of frame image indices and 
    inline LargeAnimatedSoundSequence( const DirectoryId & path, std::vector<int> & frame_indices,
                                       FrameSounds & attached_sounds, bool compressed )
        : LargeAnimatedSequence(path, frame_indices, compressed),
          SoundSequenceBase(attached_sounds) {}

    virtual ~LargeAnimatedSoundSequence() {}

protected:
    //! Updating
    virtual bool onAddingToQueue( float dt );
    
    // Stop sequence
    virtual void stop() { mIsPlaying = false; SoundSequenceBase::stop(); }
    // Set current frame
    virtual void set_frame( int frame )
        { mCurrentFrameIndex = frame; SoundSequenceBase::stop(); }
};

}
