/*
 Движок "Танита2".
 "Умные" исключения, наследованные от std::exception.
 */
#include <precompiled.h>
#include <sstream>
#include <dxerr9.h>


// Описание ошибки.
const char * winapi_error::what() const
{
    if (mDescription.empty())
    {
        // Записываем файл, строку, функцию и код ошибки.
        std::stringstream description;
        description << file() << ", " << line() << ", " << function() << ". "
                    << mFailedFunction <<  " call failed (error 0x" << std::hex << mError << "): "
                    << getErrorDescription(mError);
        mDescription = description.str();
    }
    return mDescription.c_str();
}

// Получение строки с описанием ошибки по ее коду.
const char * winapi_error::getErrorDescription( DWORD error )
{
    // Функция DXGetErrorDescription9A возвращает описание и для обычных ошибок Windows.
    return DXGetErrorDescription9A(static_cast<HRESULT>(error));
}
