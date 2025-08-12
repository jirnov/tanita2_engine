#include <precompiled.h>
#include "Application_.h"
#include "Graphics_.h"
#include "Python_.h"
#include "resource.h"
#include "Engine/sound/manager.h"
#include <tanita2/utility/singleton.h>
#include <tanita2/application/application.h>

// Application activity flag
bool DeprecatedApplicationInstance::active = true;
// Game pause flag
bool DeprecatedApplicationInstance::paused = false;
// Key states
bool DeprecatedApplicationInstance::key_states[256];
// Cursor position
D3DXVECTOR2 DeprecatedApplicationInstance::cursor_position;
//! Cursor acceleration
D3DXVECTOR2 DeprecatedApplicationInstance::smCursorAcceleration = D3DXVECTOR2(1, 1);
// Mouse button state
int DeprecatedApplicationInstance::mouse_button_state;
// Flag indicating that window may use application class
bool DeprecatedApplicationInstance::window_may_use_app;
// Game is to be quit flag
bool DeprecatedApplicationInstance::quit_game = false;
// Previous time value
DWORD DeprecatedApplicationInstance::previous_ticks = GetTickCount();

// Window function
LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

// Creates window and initializes other systems.
#define CLASS_NAME L"Tanita2GameApplication"
DeprecatedApplicationInstance::DeprecatedApplicationInstance()
    : application_time(0), application_dt(0)
{
    // Python initialization
    python.doCreate();
    // Configuration database initialization
    config.doCreate();
    // File manager initialization
    file_manager.doCreate();
}

void DeprecatedApplicationInstance::afterGfxInit()
{
    // Initializing DirectSound
    sound_manager.doCreate();
    //sound_manager->doStart();

    // Initializing DirectInput
    mouse_button_state = 0;
    input.doCreate();
}

// Process window messages
bool DeprecatedApplicationInstance::process_messages()
{
    MSG msg;
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        if (WM_QUIT == msg.message)
            return false;
    }
    return true;
}

// Game loop
void DeprecatedApplicationInstance::run()
{
    // Notifying window that application was initialized
    window_may_use_app = true;

    while (process_messages() && !quit_game)
    {
        // Updating timers and calculating dt
        DWORD ticks = GetTickCount();
        DWORD dt = ticks - previous_ticks;
        
        // Don't work while inactive
        if (!active)
        {
            previous_ticks = ticks;
            sound_manager->doSkipUpdate();
            continue;
        }
        
        // No update while paused
        if (paused)
            dt = 0;

        // Checking if Direct3D devices were lost
        if (check_lost_devices())
            continue;
        
        // Incrementing application time
        application_time += dt;
        application_dt = dt;
        
        // Saving time
        previous_ticks = ticks;

        const int fps_period = 5000;
        static DWORD last_time, fps;

        if (dt > 0)
        {
            // Updating input system
            input->update(dt);

            // Updating other systems
            utility::singleton<Direct3DInstance>()->update(dt);
            file_manager->doUpdate(dt);

            // Update and drawing on each frame
            on_frame(dt);

            sound_manager->doUpdate(dt);

            utility::singleton<render::Device>()->onFrame(dt);

            // Fps info in editor mode is not useful because it's fixed
            // FPS calculation
            if (application_time - last_time > fps_period)
            {
                using namespace boost;
                Config cfg;
                Direct3D d3d;
            
                logger()->profile() << "Fps: " << (fps * 1000 / fps_period)
                                    << ". Available video memory: "
                                    << d3d->device->GetAvailableTextureMem() / 1048576 << " Mb";
            
                last_time = application_time;
                fps = 0;
            }
            fps++;
        }
        else
        {
            Sleep(0);
        }

    }
    
    // Notifying window that application is to be destroyed
    window_may_use_app = false;
}

// Window activation handler
void DeprecatedApplicationInstance::on_activate( bool become_active )
{
    ZeroMemory(key_states, sizeof(key_states));
    // In editor mode application doesn't need to (un)acquire mouse

    if (become_active)
        input->acquire();
    else
        input->unacquire();
    pause(!become_active);

    mouse_button_state = 0;
}

// Check if Direct3D devices were lost
bool DeprecatedApplicationInstance::check_lost_devices()
{
    Direct3D d3d;
    DirectInput input;
    return d3d->is_lost() || input->is_lost();
}

// Keyboard input handler
void DeprecatedApplicationInstance::on_key_press( int keycode, bool pressed )
{
    python::Python py;
    
    // Calling engine key processing function
    if (pressed && !key_states[keycode])
        bp::call<void>(bp::object(py["Lib"].attr("engine").attr("on_keypress")).ptr(), keycode);
    key_states[keycode] = pressed;
}

// Called on script reloading
void DeprecatedApplicationInstance::on_script_reload()
{
    Direct3D d3d;
    d3d->clear_render_queue();
    
    ResourceManagerSingleton rm;
    rm->doGarbageCollection();
    
    SoundManager soundManager;
    soundManager->doUpdate(0);
}

// Local initialization
void DeprecatedApplicationInstance::init()
{
    // Getting script engine on_frame function object
    python::Python py;
    py_on_frame = py["Lib"].attr("engine").attr("on_frame");
   
    try
    {
        // Initializing python engine
        bp::call<void>(bp::object(py["Lib"].attr("engine").attr("on_init")).ptr(),
                           long(application::Application::window()));
    }
    catch (bp::error_already_set &)
    {
        PyErr_Print();
        python::Python py;
        py->traceback();
        throw;
    }
}

// Update and redraw
void DeprecatedApplicationInstance::on_frame( DWORD dt, bool just_redraw )
{
    if (just_redraw) return;
    try
    {
        // Updating game state
        bp::call<void>(py_on_frame.ptr(), dt / 1000.0f, just_redraw, 
                           cursor_position, mouse_button_state);
    }
    catch (bp::error_already_set &)
    {
        PyErr_Print();
        python::Python py;
        py->traceback();
        throw;
    }
    
    // Rendering
    utility::singleton<Direct3DInstance>()->clear(D3DCOLOR_XRGB(255, 255, 0));
    utility::singleton<Direct3DInstance>()->present();
}

// Resource releasing and deinitialization.
DeprecatedApplicationInstance::~DeprecatedApplicationInstance()
{
    window_may_use_app = false;
    
    // Cleaning python engine
    python::Python py;
    bp::call<void>(bp::object(py["Lib"].attr("engine").attr("on_cleanup")).ptr());
}
#undef CLASS_NAME

// Display python traceback message box
void DeprecatedApplicationInstance::display_traceback( const std::string & /* traceback */ ) {}
