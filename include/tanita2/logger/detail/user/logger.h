/*
 logger
 Гибкая библиотека для протоколирования работы программы.
 */
#ifndef _LOGGER_USER_LOGGER_H_
#define _LOGGER_USER_LOGGER_H_

#include <tanita2/logger/detail/logger.h>
#include <tanita2/logger/detail/user/stream.h>

namespace logger_ { namespace detail
{

// Класс с предопределенными потоками из logger/user/stream.h
template <class buffer_type>
class logger : public logger_base<buffer_type>
{
public:
    logger()
        : logger_base<buffer_type>()
    {
    }

    explicit logger( const typename buffer_type::initializer & initializer )
        : logger_base<buffer_type>(initializer)
    {
    }

// Макрос для объявления потоков.
#define LOGGER_DECLARE_STREAM(name)                                            \
    typedef typename logger_base<buffer_type>::template                        \
        internal_stream<stream::name::style_type> name##_stream_type;          \
    name##_stream_type name() const                                            \
        { return logger_base<buffer_type>::template                            \
                     user_stream<stream::name>(); }                            \
    void enable_##name##_stream( bool is_enabled )                             \
        { logger_base<buffer_type>::template                                   \
              enable_stream<stream::name>(is_enabled); }                       \
    bool name##_stream_enabled() const                                         \
        { return logger_base<buffer_type>::template                            \
              is_stream_enabled<stream::name>(); }
    
    LOGGER_DECLARE_STREAM(profile);
    LOGGER_DECLARE_STREAM(debug);
    LOGGER_DECLARE_STREAM(info);
    LOGGER_DECLARE_STREAM(warn);
    LOGGER_DECLARE_STREAM(traceback);
    LOGGER_DECLARE_STREAM(error);
    LOGGER_DECLARE_STREAM(critical);

#undef LOGGER_DECLARE_STREAM
};

}} // namespace logger_::detail

#endif // _LOGGER_USER_LOGGER_H_
