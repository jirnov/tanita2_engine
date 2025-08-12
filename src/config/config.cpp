/*
 ������ "������2".
 ����������� ��������� �������� ����������.
 */
#include <precompiled.h>
#include <tanita2/config/config.h>

namespace config
{

// ������������� ����������� ���������. ����� ����� ������ � cpp-������,
// ����� �� ���� �������������� ����� �������.
#define CONFIG_STATIC_VALUE_DEFINITION(name, value_type, value)    \
    value_type name##_type::smValue = value;


// �������� ����������� ��������.
CONFIG_STATIC_VALUE_DEFINITION(projectName, const wchar_t *, L"Tanita2 Engine Refactored");
CONFIG_STATIC_VALUE_DEFINITION(logFileName, const wchar_t *, L"journal");


#undef CONFIG_STATIC_VALUE_DEFINITION

} // namespace config
