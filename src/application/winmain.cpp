/*
 Движок "Танита2".
 Функция WinMain и обработка исключений Windows.
 */
#include <precompiled.h>
#pragma warning(push)
#pragma warning(disable:4512 4265 4365) // Предупреждение о невозможности генерации operator =.
#include <boost/program_options.hpp>
#pragma warning(pop)
#include <iostream>
#include <shellapi.h>
#include <winnls.h>
#include <tanita2/application/application.h>
#include <tanita2/config/config.h>

namespace
{

// Глобальный объект-логгер.
utility::manual_singleton<logger_type> gLogger;


// Возвращает путь к каталогу с исполняемым файлом.
std::wstring getExecutableDirectory( const wchar_t * exePath )
{
    wchar_t drive[_MAX_DRIVE], dir[_MAX_DIR],
            fname[_MAX_FNAME], ext[_MAX_EXT];
    _wsplitpath(exePath, drive, dir, fname, ext);
    
    assert(_MAX_DRIVE + _MAX_DIR <= _MAX_PATH);
    wchar_t path[_MAX_PATH];
    _wmakepath(path, drive, dir, NULL, NULL);
    
    wchar_t fullpath[_MAX_PATH];
    _wfullpath(fullpath, path, _MAX_PATH);
    return std::wstring(fullpath);
}

// Вызывается из WinMain, может бросать исключения без ущерба диагностике.
void ExceptionGuardedWinMain( HINSTANCE instance )
{
    // Обрабатываем аргументы командной строки.
    int argc;
    wchar_t ** argv = CommandLineToArgvW(GetCommandLine(), &argc);
    
    namespace po = boost::program_options;
    po::options_description opt("Options");
    std::wstring workingDirectory;
    opt.add_options()
        ("help", "Print command line help")
        
        ("working-directory,w", po::wvalue<std::wstring>(&workingDirectory),
             "Working directory (defaults to path of executable file).")
        
        ("allow-second", "Allow second instance of application.")
        ;
    po::variables_map vm;
    try
    {
        po::store(po::wcommand_line_parser(argc, argv).options(opt).run(), vm);
    }
    catch (po::error &)
    {
        std::cout << "Invalid command line parameters.\n" << opt;
        return;
    }
    po::notify(vm);
    
    if (vm.count("help"))
    {
        std::cout << opt;
        return;
    }
    if (workingDirectory.empty())
        workingDirectory = getExecutableDirectory(argv[0]);
    
    // Меняем каталог.
    check_win32(SetCurrentDirectory(workingDirectory.c_str()));
    
    
    // Проверяем, не запущен ли другой экземпляр приложения.
    HANDLE mutex = NULL;
    if (!vm.count("allow-second"))
    {
        std::wstring mutexName = std::wstring(config::projectName()) + L" Mutex";
        mutex = CreateMutex(NULL, TRUE, mutexName.c_str());
        DWORD error = GetLastError();
        if (NULL == mutex)
            throw_winapi_error(error, "CreateMutex");
        if (ERROR_ALREADY_EXISTS == error)
        {
            std::cout << "Application instance already running.\n";
            return;
        }
    }
    
    // Создаем логгер. Удаляться он будет сам, т.к. может использоваться в деструкторах.
    gLogger.doCreate(config::logFileName());
    gLogger->info() << "Engine version: " << config::projectBuildRevision();
    if (config::projectIsNotVersioned())
        gLogger->warn() << "Engine sources are not under version control.";
    gLogger->info() << "Locale: " << std::locale("").name();
    gLogger->info() << "Command line: " << GetCommandLine();
    gLogger->info() << "Working directory: " << workingDirectory;
    
    
    // Область видимости для удаления объекта Application до завершения функции.
    {
        // Создаем приложение.
        utility::manual_singleton<application::Application> app;
        application::Application::Initializer init = {argv[0], instance};
        gLogger->info() << "Initializing application.";
        app.doCreate(init);
        LocalFree(argv);
        gLogger->info() << "Application initialized.";
        
        // Игровой цикл.
        gLogger->info() << "Starting main loop.";
        app->run();
        gLogger->info() << "Exiting from main loop.";
    }
    // Проверяем, что все корректно удалилось.
    assert(application::Application::correctlyDestroyed());
    gLogger->info() << "Application closed.";
    
    // Чистка мусора.
    if (NULL != mutex)
        CloseHandle(mutex);
}

} // namespace

// Функции из этого пространства будут вызываться из платформо-зависимого winmain.cpp
namespace winmain_detail
{

// Try-catch обертка вокруг ExceptionGuardedWinMain для отлова c++-исключений.
// т.к. в одной функции нельзя использовать __try и try, приходится изголяться.
void ExceptionGuardedWinMainWrapper( HINSTANCE instance )
{
    try
    {
        ExceptionGuardedWinMain(instance);
    }
    catch (std::exception & e)
    {
        std::wstringstream message;
        message << "Exception \"" << typeid(e).name() << "\": "
                << e.what() << std::endl;
        if (logger::exists())
            logger()->critical() << message;
        else
            std::cerr << utility::wstring2string(message.str());
        
        // Если идет отладка, ограничимся записью в лог.
        if (IsDebuggerPresent())
            throw;
        
        message.str(std::wstring());
        message << L"Необработанное исключение C++. Подробности в "
                << (logger::exists() ? L"лог-файле." : L"stderr.");
        application::ShowCriticalErrorDialog(message.str().c_str());
    }
}

} // namespace winmain_detail
