/*
 ������ "������2".
 ���������� ������ Application.
 */
#include <precompiled.h>
#include <tanita2/application/application.h>
#include <tanita2/config/config.h>
#include "systemInfo.h"
using namespace application;

// Handle ����������.
HINSTANCE Application::smInstance;

// �������� ����������.
Application::Application( const Application::Initializer & initializer )
    : mApplicationFileName(initializer.applicationFileName)
{
    smInstance = initializer.instance;
    logger()->enable_profile_stream(config::profileStreamEnabled());
    logger()->enable_debug_stream(config::debugStreamEnabled());
    
    // ������� ���������� � �������.
    printSystemInformation();
    
    // ������ ������.
    logger()->info() << "Initializing legacy game application.";
    mDeprecatedApp.doCreate();
    logger()->info() << "Legacy game application initialized.";
    
    // ���������� �������.
    logger()->info() << "Creating render device.";
    mRenderDevice.doCreate();
    logger()->info() << "Render device initialized.";
    
    // ������ ������ - ���������� ������������� ����� �������� �������.
    mDeprecatedApp->afterGfxInit();
}

// ������ �������� �����.
void Application::run()
{
    logger()->info() << "Configuring legacy game application.";
    mDeprecatedApp->init();
    logger()->info() << "Starting legacy main loop.";
    mDeprecatedApp->run();
}

// ����������.
Application::~Application()
{
    logger()->info() << "Application destroyed.";
}

// �������� �� ���������� ��������.
bool Application::correctlyDestroyed()
{
    bool result = true;
    result &= (!utility::manual_singleton<render::SceneGraph>::exists());
    result &= (!utility::manual_singleton<render::Device>::exists());
    result &= (!utility::manual_singleton<Application>::exists());
    result &= (NULL == window());
    return result;
}
