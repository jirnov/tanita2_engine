/*
 ������ "������2".
 ���� ����������.
 */
#pragma once
#ifndef _TANITA2_RENDER_PRIVATE_WINDOW_H_
#define _TANITA2_RENDER_PRIVATE_WINDOW_H_

#include <boost/noncopyable.hpp>

// ��������������� ����������.
LRESULT __stdcall WindowProc( HWND, UINT, WPARAM, LPARAM );

namespace render
{

// ���� ����������.
class Window : boost::noncopyable
{
    ~Window();
    Window();
    friend class utility::manual_singleton<Window>;
    
    // ���������� ������������� ����.
    inline HWND getHWND() const
    {
        return mHWND;
    }
    
    // ��������� ���������� ����.
    void reset( u32 width, u32 height, bool windowed );
    friend class Device;
    
    // ������� �������.
    LRESULT windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
    friend LRESULT __stdcall ::WindowProc( HWND, UINT, WPARAM, LPARAM );
    
    // ����� ����.
    std::wstring mClassName;
    // ������������� ����.
    HWND mHWND;
};

} // namespace render

#endif // _TANITA2_RENDER_PRIVATE_WINDOW_H_
