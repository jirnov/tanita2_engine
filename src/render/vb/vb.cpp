/*
������ "������2".
���������� ��������� ��������� �������.
*/
#include <precompiled.h>
#include <tanita2/render/vb/vb.h>

using namespace render;

// �������� ���������� ������ � ����������� ������.
UserPointerVertexBuffer VertexBufferManager::userPointerBuffer( const VertexBufferParameters & params,
                                                                u32 vertexCount )
{
    UserPointerVertexBuffer vb(new UserPointerVertexBufferResource(params, vertexCount));
    mVertexBuffers.push_back(vb);
    return vb;
}

// ���������� � ������ ����������.
void VertexBufferManager::prepareToReset()
{
    for (VertexBufferList::iterator i = mVertexBuffers.begin(); mVertexBuffers.end() != i; ++i)
        (*i)->prepareToReset();
}

// �������������� ����� ������.
void VertexBufferManager::restoreAfterReset()
{
    for (VertexBufferList::iterator i = mVertexBuffers.begin(); mVertexBuffers.end() != i; ++i)
        (*i)->restoreAfterReset();
}

// �������� �������������� ��������.
void VertexBufferManager::unloadUnusedResources()
{
    u32 unloadedCount = 0;
    for (VertexBufferList::iterator i = mVertexBuffers.begin(); mVertexBuffers.end() != i;)
    {
        if (i->unique())
        {
            i = mVertexBuffers.erase(i);
            unloadedCount++;
        }
        else
            ++i;
    }
    if (unloadedCount != 0)
        logger()->profile() << "Unloaded " << unloadedCount << " unused vertex buffers";
}
