/*
 ������ "������2".
 ��������� ����������, ����������� � ����������� �� ����� ��������.
 */
#pragma once
#ifndef _TANITA2_CONFIG_CONFIG_H_
#define _TANITA2_CONFIG_CONFIG_H_
#include <tanita2/config/detail/begin_detail.h>

namespace config
{

// ������������� ������� ������ � ������� �������� ������.
// ���������� ������������� ��� ������ ������.
CONFIG_STATIC_VALUE(projectBuildRevision, const wchar_t *);

// ������� ������ ���������� ��� �������� ��������� ���������.
CONFIG_STATIC_VALUE(projectIsNotVersioned, bool);

// �������� �������.
CONFIG_STATIC_VALUE(projectName, const wchar_t *);

// ��� ���-�����.
CONFIG_STATIC_VALUE(logFileName, const wchar_t *);

// ��������� ������� ������ �������������� � �������.
CONFIG_VALUE(profileStreamEnabled, bool, true);
CONFIG_VALUE(debugStreamEnabled, bool, true);

namespace gfx
{

// ��������� NV PerfHUD.
// TODO: �������� �� ��������� - false.
CONFIG_VALUE(perfHUDEnabled, bool, true);

} // namespace gfx

} // namespace config

#include <tanita2/config/detail/end_detail.h>
#endif // _TANITA2_CONFIG_CONFIG_H_
