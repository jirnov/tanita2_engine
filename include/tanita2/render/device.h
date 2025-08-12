/*
 ������ "������2".
 ����������� ����������.
 */
#pragma once
#ifndef _TANITA2_RENDER_DEVICE_H_
#define _TANITA2_RENDER_DEVICE_H_

#include <boost/noncopyable.hpp>
#include <d3d9.h>

namespace render
{

// ���������� �������.
class Device : boost::noncopyable
{
public:
    // ���������� ������������� ���� ��� NULL.
    static HWND window();
    
    ~Device();
    
    // ����� ���������� � ���������� ����� ����������.
    void reset( const D3DPRESENT_PARAMETERS & newParameters );
    
    // �������, ���������� �� ������ ����� ��� �������� �������������� ��������.
    void onFrame( u32 dt );
    
    // ��������� ���������� ����������.
    inline D3DPRESENT_PARAMETERS parameters() const
    {
        return mParameters;
    }
    
private:
    Device();
    friend class utility::manual_singleton<Device>;
    
    LPDIRECT3D9 mDirect3D;
    UINT mAdapter;
    D3DDEVTYPE mDeviceType;
    LPDIRECT3DDEVICE9 mDevice;
    D3DPRESENT_PARAMETERS mParameters;
    utility::manual_singleton<class Window> mWindow;
    utility::manual_singleton<class SceneGraph> mSceneGraph;
    utility::manual_singleton<class TextureManager> mTextureManager;
    utility::manual_singleton<class VertexBufferManager> mVertexBufferManager;
};

} // namespace render

#endif // _TANITA2_RENDER_DEVICE_H_
