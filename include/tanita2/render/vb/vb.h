/*
 Движок "Танита2".
 Менеджер вершинных буферов.
 */
#pragma once
#ifndef _TANITA2_RENDER_VB_VB_H_
#define _TANITA2_RENDER_VB_VB_H_

#include <tanita2/render/vb/vbtypes.h>

namespace render
{

// Менеджер вершинных буферов.
class VertexBufferManager : boost::noncopyable
{
public:
    // Создание вершинного буфера в оперативной памяти.
    UserPointerVertexBuffer userPointerBuffer( const VertexBufferParameters & params, u32 vertexCount );
    
private:
    VertexBufferManager() {}
    friend class utility::manual_singleton<VertexBufferManager>;
    
    // Подготовка к сбросу устройства и восстановление после него.
    void prepareToReset();
    void restoreAfterReset();
    // Выгрузка неиспользуемых ресурсов.
    void unloadUnusedResources();
    friend class Device;
    
    // Список вершинных буферов.
    typedef std::vector<VertexBuffer> VertexBufferList;
    VertexBufferList mVertexBuffers;
};

} // namespace render

#endif // _TANITA2_RENDER_VB_VB_H_
