/*
 Движок "Танита2".
 Класс Application - игровое приложение.
 */
#pragma once
#ifndef _TANITA2_APPLICATION_APPLICATION_H_
#define _TANITA2_APPLICATION_APPLICATION_H_

#include <tanita2/utility/singleton.h>
#include <tanita2/render/device.h>
#include <deprecated/deprecated.h>

namespace application
{

// Отображение диалогового окна с сообщением о критической ошибке.
void ShowCriticalErrorDialog( const wchar_t * message );


// Игровое приложение и окно.
class Application : boost::noncopyable
{
public:
    // Получение окна приложения.
    static inline HWND window()
    {
        return render::Device::window();
    }
    
    // Получение идентификатора приложения.
    static inline HINSTANCE instance()
    {
        return smInstance;
    }
    
    
    // Параметры для создания Application.
    struct Initializer
    {
        const wchar_t * applicationFileName;
        HINSTANCE instance;
    };
    
    // Деструктор.
    ~Application();
    
    // Запуск игрового цикла.
    void run();

    // Проверка на корректное удаление приложения и его подсистем.
    // Должна вызываться после уничтожения синглтона Application.
    static bool correctlyDestroyed();

private:
    Application( const Initializer & initializer );
    friend class utility::manual_singleton<Application>;
    
    // Идентификатор приложения.
    static HINSTANCE smInstance;
    // Путь к исполняемому файлу приложения.
    std::wstring mApplicationFileName;
    
    // TODO: убрать эту мэрзость.
    // NB! Эта мэрзость должна быть объявлена до рендера!!! Иначе она удалится после
    // него и будет к нему обращаться.
    utility::manual_singleton<DeprecatedApplicationInstance> mDeprecatedApp;
    
    // Устройство рендера.
    utility::manual_singleton<render::Device> mRenderDevice;
};

} // namespace application

#endif // _TANITA2_APPLICATION_APPLICATION_H_
