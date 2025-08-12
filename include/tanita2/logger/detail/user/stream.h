/*
 logger
 Гибкая библиотека для протоколирования работы программы.
 */
#ifndef _LOGGER_USER_STREAM_H_
#define _LOGGER_USER_STREAM_H_

#include <tanita2/logger/detail/stream.h>
#include <tanita2/logger/detail/style.h>

namespace logger_ { namespace stream
{

// Часто используемые потоки вывода.

struct profile        : stream_base<style::profile>   { enum { is_enabled_by_default = false }; };
struct debug          : stream_base<style::debug>     {};
struct info           : stream_base<style::info>      {};
struct warn           : stream_base<style::warn>      {};
struct traceback      : stream_base<style::traceback> {};
struct error          : stream_base<style::error>     {};
struct critical       : stream_base<style::critical>  {};

}} // namespace logger_::stream

#endif // _LOGGER_USER_STREAM_H_
