/*
������ "������2".
���������� ��������� �������.
*/
#include <precompiled.h>
#include <tanita2/render/vb/vb.h>

using namespace render;

namespace
{

// ��������� ������� ������� ��� FVF.
// TODO: ������� �������������� ��������� FVF � ��� �������:
// FVF.tex0(2).xyz().normal().diffuse()
// ������ ��������� ���������� ���������� ��������� �������� ������.
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
    
    // ���� ��������� ���� assert, ������ ������������ ����, ���������������� ���� ��������.
    assert(fvf == (fvf & (D3DFVF_XYZ | D3DFVF_XYZW | D3DFVF_NORMAL | D3DFVF_DIFFUSE |
                          D3DFVF_SPECULAR | D3DFVF_TEX1)));
    return size;
}

} // namespace

// �������� ���������� ������ � ����������� ������.
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

// ����������.
UserPointerVertexBufferResource::~UserPointerVertexBufferResource()
{
    assert(!mIsLocked);
    assert(mVertexData);
    if (NULL != mVertexData)
        delete [] mVertexData;
}

// ��������� ��������� �� ������.
void * UserPointerVertexBufferResource::lock()
{
    assert(!mIsLocked);
    assert(mVertexData);
    mIsLocked = true;
    return mVertexData;
}

// ��������� ������� � ������ ������.
void UserPointerVertexBufferResource::unlock()
{
    assert(mIsLocked);
    assert(mVertexData);
    mIsLocked = false;
}

// ���������� � ������ ����������.
void UserPointerVertexBufferResource::prepareToReset()
{
}

// �������������� ����� ������.
void UserPointerVertexBufferResource::restoreAfterReset()
{
}
