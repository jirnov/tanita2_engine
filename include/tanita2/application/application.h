/*
 ������ "������2".
 ����� Application - ������� ����������.
 */
#pragma once
#ifndef _TANITA2_APPLICATION_APPLICATION_H_
#define _TANITA2_APPLICATION_APPLICATION_H_

#include <tanita2/utility/singleton.h>
#include <tanita2/render/device.h>
#include <deprecated/deprecated.h>

namespace application
{

// ����������� ����������� ���� � ���������� � ����������� ������.
void ShowCriticalErrorDialog( const wchar_t * message );


// ������� ���������� � ����.
class Application : boost::noncopyable
{
public:
    // ��������� ���� ����������.
    static inline HWND window()
    {
        return render::Device::window();
    }
    
    // ��������� �������������� ����������.
    static inline HINSTANCE instance()
    {
        return smInstance;
    }
    
    
    // ��������� ��� �������� Application.
    struct Initializer
    {
        const wchar_t * applicationFileName;
        HINSTANCE instance;
    };
    
    // ����������.
    ~Application();
    
    // ������ �������� �����.
    void run();

    // �������� �� ���������� �������� ���������� � ��� ���������.
    // ������ ���������� ����� ����������� ��������� Application.
    static bool correctlyDestroyed();

private:
    Application( const Initializer & initializer );
    friend class utility::manual_singleton<Application>;
    
    // ������������� ����������.
    static HINSTANCE smInstance;
    // ���� � ������������ ����� ����������.
    std::wstring mApplicationFileName;
    
    // TODO: ������ ��� ��������.
    // NB! ��� �������� ������ ���� ��������� �� �������!!! ����� ��� �������� �����
    // ���� � ����� � ���� ����������.
    utility::manual_singleton<DeprecatedApplicationInstance> mDeprecatedApp;
    
    // ���������� �������.
    utility::manual_singleton<render::Device> mRenderDevice;
};

} // namespace application

#endif // _TANITA2_APPLICATION_APPLICATION_H_
