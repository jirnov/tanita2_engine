/*
 ������ "������2".
 ���� ����������.
 */
#include <precompiled.h>
#include <tanita2/config/config.h>
#include <tanita2/application/application.h>
#include "resource.h"
#include "window.h"

// ������� �������.
LRESULT __stdcall WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    // �������� ���������� ������ Window.
    if (utility::singleton<render::Window>::exists())
        return utility::singleton<render::Window>()->windowProc(hWnd, message, wParam, lParam);
    else
        return DefWindowProc(hWnd, message, wParam, lParam);
}

// �����������.
render::Window::Window()
    : mHWND(NULL)
{
    // ��� ������ ����.
    mClassName = std::wstring(config::projectName()) + L" Window";
    
    // ����������� �������� ������.
    WNDCLASSEX wcex;
    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_CLASSDC;
    wcex.lpfnWndProc   = WindowProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = application::Application::instance();
    wcex.hIcon         = LoadIcon(wcex.hInstance, reinterpret_cast<LPCTSTR>(IDI_APPLICATION_ICON));
    wcex.hCursor       = NULL;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = mClassName.c_str();
    wcex.hIconSm       = wcex.hIcon;
    check_win32(RegisterClassEx(&wcex));
    
    // ������� ����.
    const DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW;
    RECT adjustedSize = {0, 0, 639, 479}; // ������� ���������� ������� �� ���������.
    check_win32(AdjustWindowRect(&adjustedSize, WINDOW_STYLE, FALSE));
    mHWND = CreateWindow(wcex.lpszClassName, config::projectName(), WINDOW_STYLE,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         adjustedSize.right - adjustedSize.left + 1, 
                         adjustedSize.bottom - adjustedSize.top + 1,
                         NULL, NULL, wcex.hInstance, NULL);
    check_win32(mHWND);
}

// ����������.
render::Window::~Window()
{
    // UnregisterClass �������� ����������, �.�. ���� ��� ����������.
    // DestroyWindow �� �������� � �������� (������ - ��).
    // ������� ����� ��� ������ �������.
}

// ��������� ���������� ����.
void render::Window::reset( u32 width, u32 height, bool windowed )
{
    assert(NULL != mHWND);
    if (0 == width || 0 == height)
    {
        width = 640;
        height = 480;
    }
    RECT adjustedSize = {0, 0, width - 1, height - 1};
    const DWORD WINDOW_STYLE = windowed ? WS_OVERLAPPEDWINDOW : WS_POPUP;
    check_win32(SetWindowLong(mHWND, GWL_STYLE, WINDOW_STYLE));
    check_win32(AdjustWindowRect(&adjustedSize, WINDOW_STYLE, FALSE));
    const int windowWidth = adjustedSize.right - adjustedSize.left + 1,
              windowHeight = adjustedSize.bottom - adjustedSize.top + 1;
    // ���������� ��� ���� :)
    // ���� ������ ���� ������ ��� ����� ������� ������,
    // �������� ���� � ������� ����� ����.
    int swpFlags = SWP_FRAMECHANGED;
    if (windowWidth < GetSystemMetrics(SM_CXSCREEN))
        swpFlags |= SWP_NOMOVE; // ���� ��������� �� ������ - �� �������.
    check_win32(SetWindowPos(mHWND, HWND_TOP, 0, 0, windowWidth, windowHeight, swpFlags));
    ShowWindow(mHWND, SW_SHOW);
}

// ������� �������.
LRESULT render::Window::windowProc( HWND hWnd, UINT message,
                                    WPARAM wParam, LPARAM lParam )
{
    switch (message)
    {
    case WM_CLOSE:
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    
    // TODO: input ������ �������������� �� �����.
    case WM_KEYUP:
    case WM_KEYDOWN:
        if (utility::singleton<DeprecatedApplicationInstance>::exists() && DeprecatedApplicationInstance::window_may_use_app)
            utility::singleton<DeprecatedApplicationInstance>()->on_key_press((int)wParam, WM_KEYDOWN == message);
        return 0;
    
    // TODO: ������ ��� ��������.
    case WM_ACTIVATE:
        DeprecatedApplicationInstance::active = LOWORD(wParam) != WA_INACTIVE;
        if (utility::singleton<DeprecatedApplicationInstance>::exists() && DeprecatedApplicationInstance::window_may_use_app)
            utility::singleton<DeprecatedApplicationInstance>()->on_activate(DeprecatedApplicationInstance::active);
        return 0;
    
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
