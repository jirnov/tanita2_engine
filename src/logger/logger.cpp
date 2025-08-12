/*
 ������ "������2".
 ����������� ����������� ���������� � �������.
 */
#include <precompiled.h>
#include <tanita2/logger/detail/buffer/html.h>

// ����������� ��� ������ � ����������� ��������������.
logger_type::pre_manupulator logger::pre;

// �����������.
logger_::buffer::html::html( const logger_::buffer::html::initializer & initializer )
    : mPreformatedOutput(false)
{
    mLogFile.exceptions(std::ios::failbit);
    mLogFile.open((std::wstring(initializer.fileName) + L".html").c_str(),
                  std::ios_base::out | std::ios_base::trunc);
    
    mLogFile << // ��������� ��������� html.
                #include <tanita2/logger/detail/buffer/html/header.inc>
                ;
}

// ����������.
logger_::buffer::html::~html()
{
    mLogFile << // ���������� ��������� html-�����.
                # include <tanita2/logger/detail/buffer/html/footer.inc>
                ;
}
