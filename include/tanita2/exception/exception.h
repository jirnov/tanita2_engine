/*
 ������ "������2".
 "�����" ����������, ������������� �� std::exception.
 */
#pragma once
#ifndef _TANITA2_EXCEPTION_EXCEPTION_H_
#define _TANITA2_EXCEPTION_EXCEPTION_H_

#include <exception>
#include <boost/current_function.hpp>
#include <tanita2/exception/detail/detail.h>

//! ���������� ������ ��� throw winapi_error � ����������� � ����� �������.
#define throw_winapi_error(lastError, winapiFunctionName)              \
    (throw winapi_error(lastError, winapiFunctionName,                 \
                        __FILE__, __LINE__, BOOST_CURRENT_FUNCTION))

//! �������� �� ������ ��� ������ WinAPI.
#define check_win32(call) \
    (void)((!(call)) && (throw_winapi_error(GetLastError(), #call), 0))


//! ���������� ������ ��� throw directx_error � ����������� � ����� �������.
#define throw_directx_error(lastError, directxFunctionName)              \
    (throw directx_error(lastError, directxFunctionName,                 \
                         __FILE__, __LINE__, BOOST_CURRENT_FUNCTION))

//! �������� �� ������ ��� ������ DirectX.
#define check_directx(call)                      \
    for (;;) {                                   \
        HRESULT hr = (call);                     \
        if (FAILED(hr))                          \
            throw_directx_error(hr, #call);      \
        break;                                   \
    }



//! ���������� ��� ������ ������ ������� WinAPI.
class winapi_error : public exception_detail::exception
{
public:
    //! �����������, ����������� ��� ������.
    /**
     ������ ���������� ��������� GetLastError(). ������ ������ -
     ��� ������ ������ ������������ ����� �� ����� ������ �������������
     ������� - ����� ���������� �������� ����� ������ ��������� � ���
     ����� �������� ��� ������ � 0.
     
     \code
     if (INVALID_FILE_HANDLE == (f = CreateFile(...)))
     {
        // cout << "�� ������� ������� ����";  // ������, ��� ������
        DWORD error = GetLastError();
        cout << "�� ������� ������� ����"; // ������ ��� ���������.
        throw_winapi_error(error); // ���������� ������� ������, �������
                                   // ��������� ����, ������ � �-���.
     }
     \endcode
     */
    winapi_error( DWORD lastError, const char * failedFunction,
                  const char * file, int line, const char * function )
        : exception_detail::exception(file, line, function),
          mError(lastError), mFailedFunction(failedFunction)
        {}
    
    // �������� ������.
    const char * what() const;
    
    
    // ��������� ������ � ��������� ������ �� �� ����.
    static const char * getErrorDescription( DWORD error );
    
private:
    DWORD mError;
    std::string mFailedFunction;
    mutable std::string mDescription;
};

//! ���������� ��� ������ ������ ������� DirectX.
class directx_error : public winapi_error
{
public:
    //! �����������, ����������� ��� ������.
    directx_error( HRESULT error, const char * failedFunction,
                   const char * file, int line, const char * function )
        : winapi_error(static_cast<DWORD>(error), failedFunction, file, line, function)
        {}
    
    // �������� ������.
    using winapi_error::what;
};

#endif // _TANITA2_EXCEPTION_EXCEPTION_H_
