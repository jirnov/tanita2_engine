#include <precompiled.h>
#include "Input_.h"
#include "Graphics_.h"
#include "EffectsManager_.h"
#include "Sources/graphics/sequenceManager/sequenceManager.h"

#include "Python_.h"

#include "Application_.h"
//#include "Engine/sound/manager.h"
#include "Sequence_.h"
#include "GameObject_.h"
#include <tanita2/application/application.h>

// Application main window handle
extern HWND toplevel_window;

// Constructor
DirectInputInstance::DirectInputInstance()
     : direct_input(NULL), mouse(NULL), x(512), y(384), is_device_lost(false)
{
    // Creating DirectInput object
    check_directx(DirectInput8Create(application::Application::instance(), DIRECTINPUT_HEADER_VERSION,
                                      IID_IDirectInput8, (void **)&direct_input, NULL));
    // Creating mouse handle
    check_directx(direct_input->CreateDevice(GUID_SysMouse, &mouse, NULL));
    
    // Initializations
    Config cfg;
    width = cfg->get<int>("width");
    height = cfg->get<int>("height");
    check_directx(mouse->SetDataFormat(&c_dfDIMouse));

#define INPUT_MODE DISCL_FOREGROUND | DISCL_EXCLUSIVE
#define MainWindow application::Application::window()

    check_directx(mouse->SetCooperativeLevel(MainWindow, INPUT_MODE));
#undef INPUT_MODE
    DIPROPDWORD MProp;
    MProp.diph.dwSize = sizeof(DIPROPDWORD);
    MProp.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    MProp.diph.dwObj = 0;
    MProp.diph.dwHow = DIPH_DEVICE;
    MProp.dwData = 64;
    check_directx(mouse->SetProperty(DIPROP_BUFFERSIZE, &MProp.diph));
}

// Destructor
DirectInputInstance::~DirectInputInstance()
{
#define SAFE_RELEASE(res) { if (res) res->Release(); }
    if (mouse) mouse->Unacquire();
    SAFE_RELEASE(mouse);
    SAFE_RELEASE(direct_input);
#undef SAFE_RELEASE
}

// Acquiring mouse
void DirectInputInstance::acquire()
{
    HRESULT result = mouse->Acquire();
    if (DIERR_OTHERAPPHASPRIO != result)
    {
        check_directx(result);
    }
    else
        is_device_lost = true;
}

// Check if device was lost and restore if possible
bool DirectInputInstance::is_lost()
{
    if (!is_device_lost)
        return false;
    is_device_lost = false;
    acquire();
    return is_device_lost;
}

// Update input state
void DirectInputInstance::update( DWORD /* dt */ )
{
    DWORD elements = 1;
    DIDEVICEOBJECTDATA data;
    
    int xOld = x, yOld = y;
    DeprecatedApplicationInstance::mouse_button_state &= 7;
    for (;;)
    {
        if (FAILED(mouse->GetDeviceData(sizeof(data), &data, &elements, 0)))
        {
            acquire();
            break;
        }
        if (elements == 0)
            break;
        switch(data.dwOfs)
        {
        case DIMOFS_X:
            x += data.dwData;
            break;

        case DIMOFS_Y:
            y += data.dwData;
            break;
            
        case DIMOFS_Z:
            DeprecatedApplicationInstance::mouse_button_state |= 1 << (int(data.dwData) > 0 ? 3 : 4);
            break;

        case DIMOFS_BUTTON0:
        case DIMOFS_BUTTON1:
        case DIMOFS_BUTTON2:
        case DIMOFS_BUTTON3:
        case DIMOFS_BUTTON4:
        case DIMOFS_BUTTON5:
        case DIMOFS_BUTTON6:
        case DIMOFS_BUTTON7:
            int button = data.dwOfs - DIMOFS_BUTTON0;
            if (data.dwData & 0x80)
                DeprecatedApplicationInstance::mouse_button_state |= 1 << button;
            else
                DeprecatedApplicationInstance::mouse_button_state &= ~(1 << button);
            break;
        }
    }
    
    int dx = static_cast<int>((x - xOld) * DeprecatedApplicationInstance::smCursorAcceleration.x),
        dy = static_cast<int>((y - yOld) * DeprecatedApplicationInstance::smCursorAcceleration.y);
    x = xOld + dx;
    y = yOld + dy;
    
    // Clipping
    if (x < 0) x = 0;
    if (x > width) x = width;
    if (y < 0) y = 0;
    if (y > height) y = height;
    
    DeprecatedApplicationInstance::cursor_position.x = x * 1024.0f / width;
    DeprecatedApplicationInstance::cursor_position.y = y * 768.0f / height;
}
