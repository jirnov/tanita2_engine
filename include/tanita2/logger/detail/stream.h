/*
 logger
 Гибкая библиотека для протоколирования работы программы.
 */
#ifndef _LOGGER_STREAM_H_
#define _LOGGER_STREAM_H_

namespace logger_ { namespace stream
{

//! Базовый класс для пользовательских потоков вывода.
template<class style>
class stream_base
{
public:
    //! Стиль, используемый при выводе в поток.
    typedef style style_type;
    
    //! Включен по умолчанию.
    enum { is_enabled_by_default = true };

    /**
     Возвращает строку, записываемую в начало каждого
     выводимого в данный поток сообщения.
     */
    static const char * prefix()
    {
        return "";
    }
    
protected:
    // Конструктор доступен только детям.
    stream_base() {}
};

}} // namespace logger_::stream

#endif // _LOGGER_STREAM_H_
