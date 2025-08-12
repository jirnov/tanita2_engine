/*
 ������ "������2".
 �������� ��������� �������.
 */
#pragma once
#ifndef _TANITA2_RENDER_VB_VB_H_
#define _TANITA2_RENDER_VB_VB_H_

#include <tanita2/render/vb/vbtypes.h>

namespace render
{

// �������� ��������� �������.
class VertexBufferManager : boost::noncopyable
{
public:
    // �������� ���������� ������ � ����������� ������.
    UserPointerVertexBuffer userPointerBuffer( const VertexBufferParameters & params, u32 vertexCount );
    
private:
    VertexBufferManager() {}
    friend class utility::manual_singleton<VertexBufferManager>;
    
    // ���������� � ������ ���������� � �������������� ����� ����.
    void prepareToReset();
    void restoreAfterReset();
    // �������� �������������� ��������.
    void unloadUnusedResources();
    friend class Device;
    
    // ������ ��������� �������.
    typedef std::vector<VertexBuffer> VertexBufferList;
    VertexBufferList mVertexBuffers;
};

} // namespace render

#endif // _TANITA2_RENDER_VB_VB_H_
