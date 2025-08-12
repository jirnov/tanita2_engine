/*
 Движок "Танита2".
 "Умные" исключения, наследованные от std::exception.
 */
#pragma once
#ifndef _TANITA2_EXCEPTION_EXCEPTION_H_
#define _TANITA2_EXCEPTION_EXCEPTION_H_

#include <exception>
#include <boost/current_function.hpp>
#include <tanita2/exception/detail/detail.h>

//! Упрощенный способ для throw winapi_error с информацией о месте падения.
#define throw_winapi_error(lastError, winapiFunctionName)              \
    (throw winapi_error(lastError, winapiFunctionName,                 \
                        __FILE__, __LINE__, BOOST_CURRENT_FUNCTION))

//! Проверка на ошибку при вызове WinAPI.
#define check_win32(call) \
    (void)((!(call)) && (throw_winapi_error(GetLastError(), #call), 0))


//! Упрощенный способ для throw directx_error с информацией о месте падения.
#define throw_directx_error(lastError, directxFunctionName)              \
    (throw directx_error(lastError, directxFunctionName,                 \
                         __FILE__, __LINE__, BOOST_CURRENT_FUNCTION))

//! Проверка на ошибку при вызове DirectX.
#define check_directx(call)                      \
    for (;;) {                                   \
        HRESULT hr = (call);                     \
        if (FAILED(hr))                          \
            throw_directx_error(hr, #call);      \
        break;                                   \
    }



//! Исключение при ошибке вызова функции WinAPI.
class winapi_error : public exception_detail::exception
{
public:
    //! Конструктор, принимающий код ошибки.
    /**
     Обычно передается результат GetLastError(). Тонкий момент -
     код ошибки должен запоминаться сразу же после вызова несработавшей
     функции - любое безобидное действие вроде вывода сообщения в лог
     может сбросить код ошибки в 0.
     
     \code
     if (INVALID_FILE_HANDLE == (f = CreateFile(...)))
     {
        // cout << "Не удалось открыть файл";  // Прощай, код ошибки
        DWORD error = GetLastError();
        cout << "Не удалось открыть файл"; // Теперь все нормально.
        throw_winapi_error(error); // Используем удобный макрос, который
                                   // подставит файл, строку и ф-цию.
     }
     \endcode
     */
    winapi_error( DWORD lastError, const char * failedFunction,
                  const char * file, int line, const char * function )
        : exception_detail::exception(file, line, function),
          mError(lastError), mFailedFunction(failedFunction)
        {}
    
    // Описание ошибки.
    const char * what() const;
    
    
    // Получение строки с описанием ошибки по ее коду.
    static const char * getErrorDescription( DWORD error );
    
private:
    DWORD mError;
    std::string mFailedFunction;
    mutable std::string mDescription;
};

//! Исключение при ошибке вызова функции DirectX.
class directx_error : public winapi_error
{
public:
    //! Конструктор, принимающий код ошибки.
    directx_error( HRESULT error, const char * failedFunction,
                   const char * file, int line, const char * function )
        : winapi_error(static_cast<DWORD>(error), failedFunction, file, line, function)
        {}
    
    // Описание ошибки.
    using winapi_error::what;
};

#endif // _TANITA2_EXCEPTION_EXCEPTION_H_
