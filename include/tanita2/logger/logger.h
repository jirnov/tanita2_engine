/*
 Движок "Танита2".
 Система журналирования событий движка.
 */
#pragma once
#ifndef _TANITA2_LOGGER_LOGGER_H_
#define _TANITA2_LOGGER_LOGGER_H_

#include <tanita2/logger/detail/user/logger.h>
#include <tanita2/logger/detail/buffer/html.h>
#include <tanita2/utility/singleton.h>

// Определение логгера.
typedef logger_::detail::logger<logger_::buffer::html> logger_type;
struct logger : utility::singleton<logger_type>
{
    static logger_type::pre_manupulator pre;
};

#endif // _TANITA2_LOGGER_LOGGER_H_
