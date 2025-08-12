/*
 Движок "Танита2".
 Окно приложения.
 */
#pragma once
#ifndef _TANITA2_RENDER_PRIVATE_WINDOW_H_
#define _TANITA2_RENDER_PRIVATE_WINDOW_H_

#include <boost/noncopyable.hpp>

// Предварительное объявление.
LRESULT __stdcall WindowProc( HWND, UINT, WPARAM, LPARAM );

namespace render
{

// Окно приложения.
class Window : boost::noncopyable
{
    ~Window();
    Window();
    friend class utility::manual_singleton<Window>;
    
    // Возвращает идентификатор окна.
    inline HWND getHWND() const
    {
        return mHWND;
    }
    
    // Изменение параметров окна.
    void reset( u32 width, u32 height, bool windowed );
    friend class Device;
    
    // Оконная функция.
    LRESULT windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
    friend LRESULT __stdcall ::WindowProc( HWND, UINT, WPARAM, LPARAM );
    
    // Класс окна.
    std::wstring mClassName;
    // Идентификатор окна.
    HWND mHWND;
};

} // namespace render

#endif // _TANITA2_RENDER_PRIVATE_WINDOW_H_
