/*
Движок "Танита2".
Реализация менеджера вершинных буферов.
*/
#include <precompiled.h>
#include <tanita2/render/vb/vb.h>

using namespace render;

// Создание вершинного буфера в оперативной памяти.
UserPointerVertexBuffer VertexBufferManager::userPointerBuffer( const VertexBufferParameters & params,
                                                                u32 vertexCount )
{
    UserPointerVertexBuffer vb(new UserPointerVertexBufferResource(params, vertexCount));
    mVertexBuffers.push_back(vb);
    return vb;
}

// Подготовка к сбросу устройства.
void VertexBufferManager::prepareToReset()
{
    for (VertexBufferList::iterator i = mVertexBuffers.begin(); mVertexBuffers.end() != i; ++i)
        (*i)->prepareToReset();
}

// Восстановление после сброса.
void VertexBufferManager::restoreAfterReset()
{
    for (VertexBufferList::iterator i = mVertexBuffers.begin(); mVertexBuffers.end() != i; ++i)
        (*i)->restoreAfterReset();
}

// Выгрузка неиспользуемых ресурсов.
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
