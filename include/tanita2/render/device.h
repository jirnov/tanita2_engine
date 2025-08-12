/*
 Движок "Танита2".
 Графическое устройство.
 */
#pragma once
#ifndef _TANITA2_RENDER_DEVICE_H_
#define _TANITA2_RENDER_DEVICE_H_

#include <boost/noncopyable.hpp>
#include <d3d9.h>

namespace render
{

// Устройство рендера.
class Device : boost::noncopyable
{
public:
    // Возвращает идентификатор окна или NULL.
    static HWND window();
    
    ~Device();
    
    // Сброс устройства и применения новых параметров.
    void reset( const D3DPRESENT_PARAMETERS & newParameters );
    
    // Функция, вызываемая на каждом кадре для выгрузки неиспользуемых ресурсов.
    void onFrame( u32 dt );
    
    // Получение параметров устройства.
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
