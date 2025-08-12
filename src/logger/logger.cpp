/*
 Движок "Танита2".
 Определения статических переменных и функций.
 */
#include <precompiled.h>
#include <tanita2/logger/detail/buffer/html.h>

// Манипулятор для вывода с сохранением форматирования.
logger_type::pre_manupulator logger::pre;

// Конструктор.
logger_::buffer::html::html( const logger_::buffer::html::initializer & initializer )
    : mPreformatedOutput(false)
{
    mLogFile.exceptions(std::ios::failbit);
    mLogFile.open((std::wstring(initializer.fileName) + L".html").c_str(),
                  std::ios_base::out | std::ios_base::trunc);
    
    mLogFile << // Вставляем заголовок html.
                #include <tanita2/logger/detail/buffer/html/header.inc>
                ;
}

// Деструктор.
logger_::buffer::html::~html()
{
    mLogFile << // Записываем окончание html-файла.
                # include <tanita2/logger/detail/buffer/html/footer.inc>
                ;
}
