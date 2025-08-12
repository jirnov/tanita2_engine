/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "base/assert.h"
#include "base/types.h"
#include "graphics/sequenceManager/sequenceManager.h"
#include <tanita2/render/vb/vb.h>

using namespace graphics;

// Destructor. Releases all sequences.
SequenceManager::~SequenceManager()
{
    for (SequenceList::iterator i = mManagedSequences.begin();
         mManagedSequences.end() != i; ++i)
    {
        ASSERT(*i != NULL);
    }
}

// Releases unused sequences
void SequenceManager::doUpdate( U32 dt )
{
    static U32 timer = 0;
    timer += dt;
    
    // Timer for sequence cleanup
    if (timer < 10000)
        return;
    
    timer = 0;
    doGarbageCollection();
}

// Perform immediate collection of unused data
void SequenceManager::doGarbageCollection()
{
#ifdef _DEBUG
    U32 releasedCount = 0;
#endif
    
    // Releasing unused sequences
    for (SequenceList::iterator i = mManagedSequences.begin();
         mManagedSequences.end() != i;)
        if (!i->unique())
            ++i;
        else
        {
            assert(i->unique());
            i = mManagedSequences.erase(i);
            
#ifdef _DEBUG
            releasedCount++;
#endif
        }
    
#ifdef _DEBUG    
    logger()->debug() << "Sequence manager has released " << releasedCount << " unused sequences";
#endif
}

// Registers sequence to the list of managed objects.
SequenceRef SequenceManager::registerSequence( SequenceBase * sequence )
{
    mManagedSequences.push_back(SequenceRef(sequence));
    return mManagedSequences.back();
}

// Render all sequences from queue (without queue trashing).
void SequenceManager::renderQueue()
{
    for (std::vector<SequenceBase *>::iterator i = mQueuedSequences.begin();
         mQueuedSequences.end() != i; ++i)
        (*i)->doRender();
}


namespace
{

struct VertexFormat
{
    float x, y, z;
    float u, v;

    VertexFormat( float x, float y, float u, float v )
        : x(x), y(y), z(0), u(u), v(v) {}
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
};

} // namespace

// Создание буфера под секвенцию.
VertexBufferRef SequenceManager::create( u32 width, u32 height )
{
    render::VertexBufferParameters params;
    params.fvf = VertexFormat::FVF;
    
    utility::singleton<render::VertexBufferManager> vm;
    render::UserPointerVertexBuffer buffer = vm->userPointerBuffer(params, 4);
    void * dataPtr = buffer->lock();
    
    VertexFormat vertices[] =
    {
        VertexFormat(0,     0,         0.0f, 0.0f),
        VertexFormat(width, 0,         1.0f, 0.0f),
        VertexFormat(0,     height,    0.0f, 1.0f),
        VertexFormat(width, height,    1.0f, 1.0f),
    };
    memcpy(dataPtr, vertices, sizeof(vertices));
    buffer->unlock();
    return VertexBufferRef(new LegacyVertexBuffer(buffer));
}

SequenceManager::LegacyVertexBuffer::LegacyVertexBuffer( render::UserPointerVertexBuffer buffer )
    : mVertexBuffer(buffer)
{
}

void SequenceManager::LegacyVertexBuffer::draw()
{
    Direct3D d3d;
    check_directx(d3d->device->SetFVF(VertexFormat::FVF));
    check_directx(d3d->device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
                      mVertexBuffer->deprecated_pointer_for_perversions(), sizeof(VertexFormat)));
}
