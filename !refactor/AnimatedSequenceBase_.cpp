#include <precompiled.h>
#include "AnimatedSequenceBase_.h"

using namespace ingame;

// Constructor
AnimatedSequenceBase::AnimatedSequenceBase( std::vector<int> & frame_indices )
    : time_left(0)
{
    mIsFrameBased = true;
    
    mNumberOfFrames = frame_indices.size();
    mCurrentFrameIndex = 0;
    mIsPlaying = true;
}

// Updating
bool AnimatedSequenceBase::onAddingToQueue( float dt )
{
    mIsLoopOver = false;
    
    if (mIsPlaying)
    {
        time_left -= dt;
        while (time_left < 0)
        {
            if (!(mPlaybackFlags & SEQUENCE_FLAG_REVERSED)) // Normal animation
            {
                mCurrentFrameIndex++;
                if (mCurrentFrameIndex >= (S32)mNumberOfFrames)
                {
                    mIsLoopOver = true;
                    
                    // Stopping animation or letting it play from start
                    if (mPlaybackFlags & SEQUENCE_FLAG_LOOP)
                        mCurrentFrameIndex = 0;
                    else
                    {
                        mCurrentFrameIndex = static_cast<S32>(mNumberOfFrames - 1);
                        mIsPlaying = false;
                        time_left = 0;
                        break;
                    }
                }
            }
            else
            {
                mCurrentFrameIndex--;
                if (mCurrentFrameIndex < 0)
                {
                    mIsLoopOver = true;
                
                    // Stopping animation or letting it play from start
                    if (mPlaybackFlags & SEQUENCE_FLAG_LOOP)
                        mCurrentFrameIndex = static_cast<S32>(mNumberOfFrames - 1);
                    else
                    {
                        mCurrentFrameIndex = 0;
                        mIsPlaying = false;
                        time_left = 0;
                        break;
                    }
                }
            }
            time_left += 1.0f / mFramesPerSecond;
        }
    }
    return SequenceBase::onAddingToQueue(dt);
}
