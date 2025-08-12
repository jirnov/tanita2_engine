/*
 logger
 Гибкая библиотека для протоколирования работы программы.
 */
#ifndef _LOGGER_LOGGER_H_
#define _LOGGER_LOGGER_H_

#include <boost/noncopyable.hpp>
#include <tanita2/logger/detail/stream.h>
#include <tanita2/logger/detail/style.h>
#include <tanita2/logger/detail/time.h>

namespace logger_
{

template <class buffer_type>
class logger_base : boost::noncopyable
{
public:
    //! Манипулятор для сохранения форматирования строки до конца вывода.
    struct pre_manupulator
    {
        inline pre_manupulator & operator()() const
        {
            return *this;
        }
    };
    
    
    //! Отключаемый поток вывода для записи в буфер.
    /**
     Класс для временных объектов-потоков вывода, возвращаемых
     функцией user_stream для записи в буфер.
     */
    template <class style>
    class internal_stream
    {
    public:
        template <class T>
        internal_stream & operator << ( const T & value )
        {
            if (mIsEnabled)
                mBuffer << value;
            return *this;
        }

        ~internal_stream()
        {
            if (mIsEnabled)
                mBuffer.end(style());
        }

    private:
        // Поток не может быть включен/отключен в процессе записи в него.
        internal_stream( buffer_type & buffer, const char * prefix, bool isEnabled, TIME currentTime )
            : mBuffer(buffer), mIsEnabled(isEnabled)
        {
            if (isEnabled)
            {
                mBuffer.begin(style(), currentTime);
                mBuffer << prefix;
            }
        }
        
        // Оператор присваивания запрещен.
        internal_stream & operator =( const internal_stream & );

        buffer_type & mBuffer;
        const bool mIsEnabled;

        friend class logger_base;
    };

    //! Создает буфер для вывода сообщений для буферов без инициализатора
    logger_base()
        : mBuffer(), mStartTime(getCurrentTime())
    {
    }

    //! Создает буфер для вывода сообщений
    explicit logger_base( const typename buffer_type::initializer & initializer )
        : mBuffer(initializer), mStartTime(getCurrentTime())
    {
    }

    //! Возвращает объект для вывода в заданный поток.
    template <class stream>
    internal_stream<typename stream::style_type> user_stream() const
    {
        return internal_stream<typename stream::style_type>(mBuffer, stream::prefix(),
                                                            stream_<stream>::smIsEnabled,
                                                            getCurrentTime() - mStartTime);
    }

    //! Включает/отключает запись в заданный поток.
    template <class stream>
    void enable_stream( bool is_enabled )
    {
        stream_<stream>::smIsEnabled = is_enabled;
    }
    
    //! Проверка на включение записи в поток.
    template <class stream>
    bool is_stream_enabled() const
    {
        return stream_<stream>::smIsEnabled;
    }

private:
    // Тип-обертка для хранения флага активности потока.
    template<class stream>
    struct stream_ : stream
    {
        static bool smIsEnabled;
    };
    
    // Буфер вывода.
    mutable buffer_type mBuffer;
    
    // Время с момента создания логгера.
    const TIME mStartTime;
};

template <class buffer_type>
template<class stream>
bool logger_base<buffer_type>::stream_<stream>::smIsEnabled = stream::is_enabled_by_default;

} // namespace logger_

#endif // _LOGGER_LOGGER_H_
