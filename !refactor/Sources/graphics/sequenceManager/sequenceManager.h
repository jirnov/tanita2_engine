/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef _GRAPHICS_SEQUENCEMANAGER_H_
#define _GRAPHICS_SEQUENCEMANAGER_H_

#include "base/templates.h"
#include <tanita2/utility/refcountable.h>
#include <tanita2/render/vb/vbtypes.h>

/**
 \todo: write description of Sequence Manager system
 */

// Sequence playback flags

//! Dummy sequence flag
#define SEQUENCE_FLAG_NONE  (U32)0
//! Sequence should be played over and over again.
#define SEQUENCE_FLAG_LOOP  (U32)1
//! Sequence should be played in reversed order (from end to start).
/** Used only by frame-based sequences. */
#define SEQUENCE_FLAG_REVERSED  (U32)2
//! Sequence frames are flipped from left to right
#define SEQUENCE_FLAG_FLIPPED_HORIZONTALLY (U32)4
//! Sequence frames are flipped from top to bottom
#define SEQUENCE_FLAG_FLIPPED_VERTICALLY   (U32)8

// Forward declaration
void exportSequenceBase( void );

namespace graphics
{

/**
 Base class for all sequences. Sequences are the only
 renderable (visible) objects.
 
 This class provides definitions common for all types
 of sequences and some special for frame-based animations.
 This is done intentionally to prevent use of lots of virtual
 functions.
 
 Some methods only make sense for frame-based sequences, however
 they will not fail for other sequences (but will return predefined
 invalid values).
 */
class SequenceBase : public utility::refcountable
{
public:
    using utility::refcountable::refcount;
    
    //! Default constructor
    SequenceBase();
    //! Destructor for making class abstract
    virtual ~SequenceBase() = 0 {}
    
    //! Get sequence position
    inline const vec2 & getPosition() const { return mPosition; }
    //! Set sequence position
    inline void setPosition( const vec2 & pos ) { mPosition = pos; }
    
    
    //! Get sequence bounding box dimensions
    /**
     Bounding box parameters are local to sequence (e.g. not 
     affected by parent scale/rotation).
     */
    inline const vec2 & getBoundingBox() const { return mBoundingBox; }
    
    
    //! Get number of frames per second count
    /**
     Returns number of frames per second or zero for
     non-frame-based sequences.
     */
    inline U32 getFramesPerSecond() const
        { return mIsFrameBased ? mFramesPerSecond : 0; }
    //! Set number of frames per second
    /**
     Frames per second value may be changed while sequence playback
     and will be used immediately.
     
     Doesn't work for non-frame-based sequences.
     */
    inline void setFramesPerSecond( U32 fps ) { mFramesPerSecond = fps; }
    
    
    //! Get zero-based index of current frame
    /**
     Returns positive zero-based index of current frame or
     zero for non-frame-based sequences.
     
     For reversed sequences frame #0 is a last frame of 
     non-reversed sequence.
     */
    inline U32 getCurrentFrameIndex() const
        { return mIsFrameBased ? static_cast<U32>(mCurrentFrameIndex) : 0; }
    //! Rewind sequence to given frame index
    /**
     Rewinds sequence to given zero-based frame index.
     Index may be negative, thus it will be related to
     sequence end (-1 for last frame, etc). Large positive
     and negative indices (greater then number of frames)
     are correctly wrapped.
     
     For reversed sequences frame #0 is a last frame of 
     non-reversed sequence.
     
     Sequence may be rewinded while playing, thus it will
     be stopped and mIsLoopOver flag will be reset.
     
     Doesn't work for non-frame-based sequences.
     */
    void setCurrentFrameIndex( S32 index );
    
    
    //! Returns number of frames in frame-based sequence
    /**
     Returns number of frames in sequence or zero for 
     non-frame-based sequences.
     */
    inline U32 getNumberOfFrames() const
        { return mIsFrameBased ? mNumberOfFrames : 0; }
    
    
    //! Get playback flags
    inline U32 getPlaybackFlags() const { return mPlaybackFlags; }
    /**
     Individual playback flag getting/setting functions.
     Flags can be changes on the fly (while animation playing).
     Some flags affect only frame-based sequences.
     */
#define FLAG_GETSET_FUNCTIONS(flagShortName, flagMacro)                                 \
    inline bool getIs##flagShortName() const { return getSingleFlag(flagMacro); }        \
    inline void setIs##flagShortName( bool value ) { setSingleFlag(flagMacro, value); }
    
    FLAG_GETSET_FUNCTIONS(Looped,   SEQUENCE_FLAG_LOOP)
    FLAG_GETSET_FUNCTIONS(Reversed, SEQUENCE_FLAG_REVERSED)
    FLAG_GETSET_FUNCTIONS(HFlipped, SEQUENCE_FLAG_FLIPPED_HORIZONTALLY)
    FLAG_GETSET_FUNCTIONS(VFlipped, SEQUENCE_FLAG_FLIPPED_VERTICALLY)
#undef FLAG_GETSET_FUNCTIONS
    
    
    //! Check is sequence is playing
    inline bool getIsPlaying() const { return mIsPlaying; }
    //! Start sequence playback from current position
    /**
     (Re)starts sequence playback from current frame index
     (if sequence supports repositioning, otherwise from beginning).
     */
    inline void doPlay() { mIsPlaying = true; }
    //! Stop sequence playback
    /**
     Stops sequence playback but leaves current frame index unchanged.
     Doesn't affect the mIsLoopOver flag.
     */
    virtual void doStop() { mIsPlaying = false; }
    
    
    //! Check if sequence has passed the last frame.
    /**
     This flag is set when sequence has passes the last frame (or
     frame 0 if reversed).
     
     If sequence is not looped, it is stopped (getIsPlaying() returns
     false), otherwise it is played from the beginning.
     */
    inline bool getIsLoopOver() const { return mIsLoopOver; }
    
    
    //! Adds sequence to the render queue
    void addToRenderQueue( F32 dt );
    
    
    //! Check if point (in screen coordinates) is inside sequence bounding box
    /**
     Checks if screen-space point is inside sequence's bounding box.
     A note of warning: check uses sequence transformation matrix which
     is updated once per frame on sequence update. If this method is
     called before sequence update, previous-frame matrix will be used
     (may result in invalid result for fast moving objects)
     */
    bool isPointInside( const vec2 & point );
    
    
    //! Update current transformation matrix
    /**
     Calculates transformation matrix and its inverse.
     This function is called internally and while Z-region
     processing, so it is left public.
     */
    void updateTransformationMatrix();
    
    

private:
    friend void ::exportSequenceBase( void );
    
    
protected:    
    // Called when sequence is to be added to render queue
    // This method should update sequence state and return 
    // true if it should be added to render queue.
    virtual bool onAddingToQueue( F32 /* dt */ )
        { updateTransformationMatrix();
          return checkVisibility(); }
    
    // Called by sequence manager to perform actual rendering
    virtual void doRender() {}
    
    // Used by effects pipeline to get current sequence texture
    //! \todo: decide if this method should be placed somewhere else
    //virtual ::TextureRef getCurrentTexture() = 0;
    
    
private:
    // Copy constructor and assignment operator are forbidden
    SequenceBase( const SequenceBase & );
    void operator =( const SequenceBase & );
    
    // Returns given playback flag value
    inline bool getSingleFlag( U32 flag ) const { return mPlaybackFlags & flag; }
    // Sets given playback flag to given value
    inline void setSingleFlag( U32 flag, bool value )
        { value ? mPlaybackFlags |= flag :
                  mPlaybackFlags &= ~flag; }
    
protected:
    // Sequence is a frame-based (has fps, frame index etc)
    bool mIsFrameBased;
    // Position (in local coordinates)
    vec2 mPosition;
    // Bounding box
    vec2 mBoundingBox;
    // Transformation matrix (for transition from local 
    // to screen coordinates)
    D3DXMATRIX mTransformationMatrix;
    // Inverse of transformation matrix (for transition from screen 
    // to local coordinates)
    D3DXMATRIX mInverseTransformationMatrix;
    
    // Animation frames per second count.
    U32 mFramesPerSecond;
    // Zero-based index of current frame.
    S32 mCurrentFrameIndex;
    // Number of frames in sequence.
    U32 mNumberOfFrames;
    
    // Animation playback flags (see SEQUENCE_FLAG_XXX constants)
    U32 mPlaybackFlags;
    // Animation playback status flag
    bool mIsPlaying;
    // Flag for checking loop end for looped sequences.
    bool mIsLoopOver;
    
    
    // Check if sequence is visible on screen
    bool checkVisibility();
    
    
    // Friends
    friend class SequenceManager;
};

//! Sequence reference type
typedef utility::reference<SequenceBase> SequenceRef;

/**
 Sequence manager keeps list of created sequences 
 and queue of sequences being rendered on current frame.
 
 It automatically releases sequences with zero references.
 */
class SequenceManager
{
public:
    //! Destructor. Releases all sequences.
    ~SequenceManager();
    
    //! Releases unused sequences
    void doUpdate( U32 dt );
    
    //! Perform immediate collection of unused data
    void doGarbageCollection();
    
    //! Register sequence to the list of managed objects.
    /**
     Registers pointer to sequence inside internal list of managed sequences.
     Pointer will be deleted by SequenceManager, not by user.
     */
    SequenceRef registerSequence( SequenceBase * sequence );
    
    //! Clear rendering queue (without rendering)
    inline void clearQueue() { mQueuedSequences.clear(); }
    
    //! Flush (render all and clear) sequence queue.
    inline void flushQueue() { renderQueue(); clearQueue(); }
    
    //! Render all sequences from queue (without queue trashing).
    void renderQueue();
    
    
    // Конечно, не лучшее место, но пока сойдет.
    class LegacyVertexBuffer : boost::noncopyable
    {
    public:
        void draw();
        
    private:
        LegacyVertexBuffer( render::UserPointerVertexBuffer buffer );
        friend class SequenceManager;
        
        render::UserPointerVertexBuffer mVertexBuffer;
    };
    typedef boost::shared_ptr<LegacyVertexBuffer> VertexBufferRef;
    
    VertexBufferRef create( u32 width, u32 height );
    

private:
    // Type for sequence list
    typedef std::vector<SequenceRef> SequenceList;
    // List of managed objects
    SequenceList mManagedSequences;
    // Queue of sequences to render
    std::vector<SequenceBase *> mQueuedSequences;
    
    
    // Add sequence to rendering queue
    inline void addToRenderQueue( SequenceBase * ref )
        { mQueuedSequences.push_back(ref); }
    
    
    // Friend
    friend class SequenceBase;
};

};

//! Type definition for SequenceManager singleton
typedef utility::singleton<graphics::SequenceManager> SequenceManagerSingleton;
typedef graphics::SequenceManager::VertexBufferRef VertexBufferRef;

#endif // _GRAPHICS_SEQUENCEMANAGER_H_
