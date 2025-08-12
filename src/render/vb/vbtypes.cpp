/*
Движок "Танита2".
Реализация вершинных буферов.
*/
#include <precompiled.h>
#include <tanita2/render/vb/vb.h>

using namespace render;

namespace
{

// Получение размера вершины для FVF.
// TODO: Сделать автоматическую генерацию FVF и его размера:
// FVF.tex0(2).xyz().normal().diffuse()
// Сейчас вычислить количество текстурных координат довольно сложно.
u32 getVertexSize( u32 fvf )
{
    u32 size = 0;
    
    if (fvf & D3DFVF_XYZ)
        size += 3 * sizeof(f32);
    if (fvf & D3DFVF_XYZW)
        size += 4 * sizeof(f32);
    
    if (fvf & D3DFVF_NORMAL)
        size += 3 * sizeof(f32);
    
    if (fvf & D3DFVF_DIFFUSE)
        size += sizeof(u32);
    if (fvf & D3DFVF_SPECULAR)
        size += sizeof(u32);
    
    if (fvf & D3DFVF_TEX1)
        size += 2 * sizeof(float);
    
    // Если сработает этот assert, значит используется флаг, неподдерживаемый этой функцией.
    assert(fvf == (fvf & (D3DFVF_XYZ | D3DFVF_XYZW | D3DFVF_NORMAL | D3DFVF_DIFFUSE |
                          D3DFVF_SPECULAR | D3DFVF_TEX1)));
    return size;
}

} // namespace

// Создание вершинного буфера в оперативной памяти.
UserPointerVertexBufferResource::UserPointerVertexBufferResource( const VertexBufferParameters & params,
                                                                  u32 vertexCount )
    : VertexBufferResource(params), mIsLocked(false), mVertexData(NULL)
{
    assert(NULL != vertexCount);
    assert(0 != params.fvf);
    
    u32 vertexSize = getVertexSize(params.fvf);
    assert(NULL != vertexSize);
    mVertexData = new u8[vertexCount * vertexSize];
}

// Деструктор.
UserPointerVertexBufferResource::~UserPointerVertexBufferResource()
{
    assert(!mIsLocked);
    assert(mVertexData);
    if (NULL != mVertexData)
        delete [] mVertexData;
}

// Получение указателя на данные.
void * UserPointerVertexBufferResource::lock()
{
    assert(!mIsLocked);
    assert(mVertexData);
    mIsLocked = true;
    return mVertexData;
}

// Окончание доступа к данным буфера.
void UserPointerVertexBufferResource::unlock()
{
    assert(mIsLocked);
    assert(mVertexData);
    mIsLocked = false;
}

// Подготовка к сбросу устройства.
void UserPointerVertexBufferResource::prepareToReset()
{
}

// Восстановление после сброса.
void UserPointerVertexBufferResource::restoreAfterReset()
{
}
