/*
 ������ "������2".
 ������ � ����������� � ����������� ������.
 */
#include <precompiled.h>
#include <tanita2/application/application.h>
#include <resource.h>

namespace
{

// ��������� �� ������, ��������� � �������.
std::wstring gErrorString;

// ������� ������� �������.
INT_PTR __stdcall dialogProc( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg)
    {
    case WM_INITDIALOG:
        SetDlgItemText(dlg, ID_ERRORMSG, gErrorString.c_str());
        return TRUE;
    
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            EndDialog(dlg, wParam);
            return 1;
        }
    }
    return 0;
}

} // namespace


// ����������� ����������� ���� � ���������� � ����������� ������.
// TODO: ������� ������ �������� �� ������, � ������������� ��� �����������.
void application::ShowCriticalErrorDialog( const wchar_t * message )
{
    static bool wasShown = false;
    if (wasShown)
    {
        // �������� ���� ������ �� ����������.
        if (logger::exists())
            logger()->warn() << "Critical error dialog box called twice.";
        return;
    }
    
    gErrorString = std::wstring(L"��������� �������������� ������. ") + message + 
                   L"\r\n���������� ��������� ������.";
    if (-1 == DialogBox(Application::instance(), MAKEINTRESOURCE(IDD_FAILDIALOG),
                        Application::window(), dialogProc))
    {
        DWORD error = GetLastError();
        logger()->error() << "Unable to create error dialog (error" << error << ')';
        MessageBox(Application::window(), message, L"Tanita2 Engine", MB_OK | MB_ICONERROR);
    }
    wasShown = true;
}
