/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */
#include <precompiled.h>
#include "base/baseDeclarations.h"
#include "graphics/graphics.h"
#include "graphics/sequenceManager/sequenceManager.h"

void exportSequenceBase( void )
{
    using namespace bp;
    using namespace graphics;

    class_<SequenceBase, SequenceRef, boost::noncopyable>("Sequence", no_init)
        .def("render",     &SequenceBase::addToRenderQueue)
        .def("is_inside",  &SequenceBase::isPointInside)
        .def("play",       &SequenceBase::doPlay)
        .def("stop",       &SequenceBase::doStop)
        
        .add_property("position", make_function(&SequenceBase::getPosition,
                      return_internal_reference<>()), &SequenceBase::setPosition)
        .add_property("bounding_box", make_function(&SequenceBase::getBoundingBox,
                      return_internal_reference<>()))
        .add_property("fps",      &SequenceBase::getFramesPerSecond,
                                  &SequenceBase::setFramesPerSecond)
        .add_property("frame",    &SequenceBase::getCurrentFrameIndex,
                                  &SequenceBase::setCurrentFrameIndex)
        .add_property("looped",   &SequenceBase::getIsLooped,
                                  &SequenceBase::setIsLooped)
        .add_property("reversed", &SequenceBase::getIsReversed,
                                  &SequenceBase::setIsReversed)
        .add_property("vertical_flip",   &SequenceBase::getIsVFlipped,
                                         &SequenceBase::setIsVFlipped)
        .add_property("horizontal_flip", &SequenceBase::getIsHFlipped,
                                         &SequenceBase::setIsHFlipped)
        .add_property("frame_count", &SequenceBase::getNumberOfFrames)
        .add_property("is_playing",  &SequenceBase::getIsPlaying)
        .add_property("is_over",     &SequenceBase::getIsLoopOver)
        ;
}

using namespace graphics;

// Default constructor
SequenceBase::SequenceBase()
    : mIsFrameBased(false), mPosition(0, 0), mBoundingBox(0, 0),
      mFramesPerSecond(0), mCurrentFrameIndex(0),
      mNumberOfFrames(0), mPlaybackFlags(SEQUENCE_FLAG_NONE),
      mIsPlaying(false), mIsLoopOver(false)
{}

// Rewind sequence to given frame index
void SequenceBase::setCurrentFrameIndex( S32 index )
{
    if (!mIsFrameBased)
        return;
    
    if (index >= 0)
    {
        while ((U32)index >= mNumberOfFrames)
            index -= mNumberOfFrames;
    }
    else
    {
        do
            index += mNumberOfFrames;
        while (index < 0);
    }
    
    if (mPlaybackFlags & SEQUENCE_FLAG_REVERSED)
        index = mNumberOfFrames - (index + 1);
    
    mCurrentFrameIndex = index;
    mIsLoopOver = false;
    mIsPlaying = false;
}

// Adds sequence to the render queue
void SequenceBase::addToRenderQueue( F32 dt )
{
    if (!onAddingToQueue(dt))
        return;
    
    SequenceManagerSingleton sm;
    sm->addToRenderQueue(this);
}

// Check if point (in screen coordinates) is inside sequence bounding box
bool SequenceBase::isPointInside( const vec2 & point )
{
    D3DXVECTOR4 localPoint;
    D3DXVec2Transform(&localPoint, &point, &mInverseTransformationMatrix);
    
    return localPoint.x >= 0 && localPoint.x < mBoundingBox.x &&
           localPoint.y >= 0 && localPoint.y < mBoundingBox.y;
}

// Gets current transformation matrix from rendering
void SequenceBase::updateTransformationMatrix()
{
    bool vFlip = (mPlaybackFlags & SEQUENCE_FLAG_FLIPPED_VERTICALLY),
         hFlip = (mPlaybackFlags & SEQUENCE_FLAG_FLIPPED_HORIZONTALLY);
    D3DXMATRIX tmpTransformation, tmpInvTransformation;
    
    // Calculating sequence local transformation matrix
    // (sequence to parent space)
    if (hFlip || vFlip)
    {
        D3DXMATRIX tmpTranslation, tmpScale;
        float x = mPosition.x + (hFlip ? mBoundingBox.x : 0.0f),
              y = mPosition.y + (vFlip ? mBoundingBox.y : 0.0f);
        
        D3DXMatrixTranslation(&tmpTranslation, x, y, 0.0f);
        D3DXMatrixScaling(&tmpScale, hFlip ? -1.0f : 1.0f, vFlip ? -1.0f : 1.0f, 1.0f);
        D3DXMatrixMultiply(&tmpTransformation, &tmpScale, &tmpTranslation);
        
        D3DXMatrixTranslation(&tmpTranslation, -x, -y, 0.0f);
        D3DXMatrixMultiply(&tmpInvTransformation, &tmpTranslation, &tmpScale);
    }
    else
    {
        D3DXMatrixTranslation(&tmpTransformation,     mPosition.x,  mPosition.y, 0.0f);
        D3DXMatrixTranslation(&tmpInvTransformation, -mPosition.x, -mPosition.y, 0.0f);
    }
    
    // Multiplying local matrix by parent matrix
    Graphics gfx;
    D3DXMatrixMultiply(&mTransformationMatrix, &tmpTransformation,
                       &gfx->getTransformationMatrix());
    D3DXMatrixMultiply(&mInverseTransformationMatrix,
                       &gfx->getInverseTransformationMatrix(),
                       &tmpInvTransformation);
}

// Checks if sequence is visible on screen
bool SequenceBase::checkVisibility()
{
    D3DXVECTOR4 bounds[4];
    
    // Transforming sequence bounds to screen space
    D3DXVec2Transform(&bounds[0], &D3DXVECTOR2(0, mBoundingBox.y),
                      &mTransformationMatrix);
    D3DXVec2Transform(&bounds[1],  &D3DXVECTOR2(0, 0),
                      &mTransformationMatrix);
    D3DXVec2Transform(&bounds[2],    &D3DXVECTOR2(mBoundingBox.x, mBoundingBox.y),
                      &mTransformationMatrix);
    D3DXVec2Transform(&bounds[3], &D3DXVECTOR2(mBoundingBox.x, 0),
                      &mTransformationMatrix);
    
    // Calculating screen bounds
    float left = bounds[0].x, right = left,
          top = bounds[0].y,  bottom = top;
    
    D3DXVECTOR4 * boundsPtr = bounds + 1;
    for (U32 index = 1; index < 4; ++index)
    {
        if (left > boundsPtr->x)
            left = boundsPtr->x;
        if (right < boundsPtr->x)
            right = boundsPtr->x;
        
        if (top > boundsPtr->y)
            top = boundsPtr->y;
        if (bottom < boundsPtr->y)
            bottom = boundsPtr->y;
        boundsPtr++;
    }
    return right >= 0  && left < config::gfx::SCREEN_WIDTH &&
           bottom >= 0 && top < config::gfx::SCREEN_HEIGHT;
}


// Exposing sequence to python


// Smart pointer to SequenceBase converter
namespace boost { namespace python
{

template<class T>
inline T * get_pointer( const utility::reference<T> & p ) { return const_cast<T *>(&*p); }

// Smart pointer to SequenceBase pointee
template <class T>
struct bp::pointee<utility::reference<T> >
{
    typedef T type;
};

}}
