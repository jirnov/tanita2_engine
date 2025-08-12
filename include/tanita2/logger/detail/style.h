/*
 logger
 Гибкая библиотека для протоколирования работы программы.
 */
#ifndef _LOGGER_STYLE_H_
#define _LOGGER_STYLE_H_

namespace logger_ { namespace style {

// Предопределенные стили.

struct profile {};
struct debug {};
struct info {};
struct warn {};
struct traceback {};
struct error {};
struct critical {};

}} // namespace logger_::style

#endif // _LOGGER_STYLE_H_
