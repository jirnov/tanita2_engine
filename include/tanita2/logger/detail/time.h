/*
 logger
 ������ ���������� ��� ���������������� ������ ���������.
 */
#ifndef _LOGGER_TIME_H_
#define _LOGGER_TIME_H_

#include <time.h>

namespace logger_
{

// ��� ��� ������������� ������� � ������� �������� �������.
typedef unsigned long TIME;

// �������, ������������ ������� �����.
#ifndef LOGGER_GETCURRENTTIME_DEFINED
    inline TIME getCurrentTime()
    {
        return static_cast<TIME>(clock());
    }
#endif // LOGGER_GETCURRENTTIME_DEFINED

} // namespace logger_

#endif // _LOGGER_TIME_H_
