/*
 Движок "Танита2".
 Функция WinMain и обработка исключений Windows.
 Код, специфичный для Visual C++ 2008 под x86.
 */
#include <precompiled.h>
#include <dbghelp.h>
#include <iostream>
#include <tanita2/application/application.h>

namespace
{

// Макрос для вывода строки с ошибкой в лог или cerr, если лог не создан.
// Другие способы либо не менее корявы (хоть и без макросов, но на диких шаблонах),
// либо выводят в лог в несколько строк (по строке на каждый оператор <<).
#define PRINT_ERROR(msg)               \
    if (logger::exists())              \
        logger()->critical() << msg;   \
    else                               \
        std::cerr << msg

// Функция обработки исключений.
long __stdcall topLevelExceptionHandler( EXCEPTION_POINTERS * ep )
{
    // Если это исключение С++, выводим его тип.
    if (ep->ExceptionRecord->ExceptionCode == 0xe06d7363)
    {
        // _s__ThrowInfo и _s__CatchableType - внутренние объявления компилятора.
        const _s__ThrowInfo * throwInfo =
            reinterpret_cast<_s__ThrowInfo *>(ep->ExceptionRecord->ExceptionInformation[2]);
        if (throwInfo->pCatchableTypeArray &&
            throwInfo->pCatchableTypeArray->nCatchableTypes > 0)
        {
            const _s__CatchableType * t = throwInfo->pCatchableTypeArray->arrayOfCatchableTypes[0];
            PRINT_ERROR("Unhandled C++ exception of type " << static_cast<char *>(t->pType->name));
        }
    }
    else
    {
        PRINT_ERROR("Unhandled exception (error code 0x" << std::hex << ep->ExceptionRecord->ExceptionCode << ')');
    }
    
    // Если идет отладка, креш-дамп не создаем.
    if (IsDebuggerPresent())
        return EXCEPTION_CONTINUE_SEARCH;
    
    wchar_t crashDumpFile[_MAX_PATH];
    SYSTEMTIME st;
    GetLocalTime(&st);
    _snwprintf(crashDumpFile, _MAX_PATH, L"CrashDump-%04d%02d%02d%02d%02d%02d%03d.dmp",
               st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
               st.wMilliseconds);
    
    HANDLE file = CreateFile(crashDumpFile, GENERIC_WRITE, FILE_SHARE_READ,
                             NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE != file)
    {
        HANDLE process = GetCurrentProcess();
        MINIDUMP_EXCEPTION_INFORMATION eInfo;
        eInfo.ExceptionPointers = ep;
        eInfo.ThreadId = GetCurrentThreadId();
        eInfo.ClientPointers = NULL;
        if (!MiniDumpWriteDump(process, GetProcessId(process), file,
                               MiniDumpNormal, &eInfo, NULL, NULL))
        {
            DWORD error = GetLastError();
            PRINT_ERROR("Minidump write failed with error code " << error);
        }
        CloseHandle(file);
        if (logger::exists())
            logger()->critical() << "Minidump written to " << crashDumpFile;
    }
    else
    {
        DWORD error = GetLastError();
        PRINT_ERROR("Unable to create crash dump file. Error code " << error);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
#undef PRINT_ERROR

} // namespace

namespace winmain_detail
{

// Try-catch обертка вокруг ExceptionGuardedWinMain для отлова c++-исключений.
void ExceptionGuardedWinMainWrapper( HINSTANCE instance );

} // namespace winmain_detail

// Точка входа в приложение.
int WINAPI WinMain( HINSTANCE instance, HINSTANCE, char *, int )
{
    SetUnhandledExceptionFilter(topLevelExceptionHandler);
    __try
    {
        winmain_detail::ExceptionGuardedWinMainWrapper(instance);
    }
    __except ((topLevelExceptionHandler(GetExceptionInformation()),
               IsDebuggerPresent() ? EXCEPTION_CONTINUE_SEARCH : EXCEPTION_EXECUTE_HANDLER))
    {
        application::ShowCriticalErrorDialog(L"Неопознанная ошибка. Сохранен крэш-дамп.");
    }
}
