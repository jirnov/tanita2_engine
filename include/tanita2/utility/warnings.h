/*
 ������ "������2".
 ��������� �������� � ���������� ����������� �������������� �����������.
 */
#pragma once
#ifndef _TANITA2_UTILITY_WARNINGS_H_
#define _TANITA2_UTILITY_WARNINGS_H_

// ������������ �������.
#pragma warning(push,4)

/*
 * ��������� ��������������, ����������� �� ���������.
 */

// ���� �� �������� enum �� ����������� � switch.
#pragma warning(default:4061)
// ������������ �������������� �����.
#pragma warning(default:4905 4906 4946)
// ������������� ������� � ��� �� ������, ��� � �����������.
#pragma warning(default:4263)
// �������� ����������� �������.
#pragma warning(default:4264 4266)
// ����� � ������������ �-����� ��� ������������ �����������.
#pragma warning(default:4265)
// �������� ��������� � �����������.
#pragma warning(default:4287 4296 4365)
// ������� ���������������.
#pragma warning(default:4266)
// ������ ��������.
#pragma warning(default:4555)
// ������ ������� ������� ������� �������-������.
#pragma warning(default:4347)
// ��������� ��� ���������� ��� ��� �������� �������.
#pragma warning(default:4836)

#endif // _TANITA2_UTILITY_WARNINGS_H_
