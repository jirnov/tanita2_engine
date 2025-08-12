/*
 ������ "������2".
 "�����" ����������, ������������� �� std::exception.
 */
#include <precompiled.h>
#include <sstream>
#include <dxerr9.h>


// �������� ������.
const char * winapi_error::what() const
{
    if (mDescription.empty())
    {
        // ���������� ����, ������, ������� � ��� ������.
        std::stringstream description;
        description << file() << ", " << line() << ", " << function() << ". "
                    << mFailedFunction <<  " call failed (error 0x" << std::hex << mError << "): "
                    << getErrorDescription(mError);
        mDescription = description.str();
    }
    return mDescription.c_str();
}

// ��������� ������ � ��������� ������ �� �� ����.
const char * winapi_error::getErrorDescription( DWORD error )
{
    // ������� DXGetErrorDescription9A ���������� �������� � ��� ������� ������ Windows.
    return DXGetErrorDescription9A(static_cast<HRESULT>(error));
}
