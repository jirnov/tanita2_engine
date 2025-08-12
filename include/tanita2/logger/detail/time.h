/*
 logger
 Гибкая библиотека для протоколирования работы программы.
 */
#ifndef _LOGGER_TIME_H_
#define _LOGGER_TIME_H_

#include <time.h>

namespace logger_
{

// Тип для представления времени с момента создания логгера.
typedef unsigned long TIME;

// Функция, возвращающая текущее время.
#ifndef LOGGER_GETCURRENTTIME_DEFINED
    inline TIME getCurrentTime()
    {
        return static_cast<TIME>(clock());
    }
#endif // LOGGER_GETCURRENTTIME_DEFINED

} // namespace logger_

#endif // _LOGGER_TIME_H_
