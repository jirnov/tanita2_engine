/*
 logger
 Гибкая библиотека для протоколирования работы программы.
 */
#ifndef _LOGGER_BUFFER_HTML_H_
#define _LOGGER_BUFFER_HTML_H_

#include <boost/noncopyable.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <tanita2/logger/detail/style.h>
#include <tanita2/logger/detail/time.h>
#include <tanita2/utility/encoding.h>

namespace logger_ { namespace buffer
{

//! Буфер вывода в html-файл.
class html : boost::noncopyable
{
public:
    //! Параметры для создания буфера.
    struct initializer
    {
        initializer( const wchar_t * fileName )
            : fileName(fileName)
        {
            assert(NULL != fileName);
        }
        const wchar_t * fileName;
    };

    explicit html( const initializer & initializer );

    inline void begin( const style::profile &, TIME currentTime )
    {
        write_prefix("profile", currentTime);
    }
    
    inline void begin( const style::debug &, TIME currentTime )
    {
        write_prefix("debug", currentTime);
    }

    inline void begin( const style::info &, TIME currentTime )
    {
        write_prefix("info", currentTime);
    }

    inline void begin( const style::warn &, TIME currentTime )
    {
        write_prefix("warn", currentTime);
    }

    inline void begin( const style::traceback &, TIME currentTime )
    {
        assert(!mPreformatedOutput);
        write_prefix("traceback", currentTime);
        mPreformatedOutput = true;
        mLogFile << "<pre>";
    }

    inline void begin( const style::error &, TIME currentTime )
    {
        write_prefix("error", currentTime);
    }

    inline void begin( const style::critical &, TIME currentTime )
    {
        write_prefix("critical", currentTime);
    }
    
    // Операторы вывода.
    template <class T> inline html & operator <<( const T & value );
    template <int N> inline html & operator <<( const char (& value)[N] );
    template <int N> inline html & operator <<( const wchar_t (& value)[N] );
    
    template<class style>
    void end(const style &)
    {
        if (mPreformatedOutput)
        {
            mPreformatedOutput = false;
            mLogFile << "</pre>";
        }
        mLogFile << "&nbsp;</span></div>" << std::endl;
        assert(mLogFile);
    }

    ~html();

private:
    std::string html_escape( const std::stringstream & s )
    {
        std::string result(s.str().c_str());
        boost::replace_all(result, "&", "&amp;");
        boost::replace_all(result, ">", "&gt;");
        boost::replace_all(result, "<", "&lt;");
        return result;
    }
    
    template <class T>
    html & write_escaped_html( const T & value )
    {
        std::stringstream s;
        s << value;
        mLogFile << html_escape(s);
        assert(mLogFile);
        return *this;
    }

    inline void write_prefix( const char * prefix, TIME currentTime )
    {
        mLogFile << " <div class=\"" << prefix << "\">" << "<span class=\"time\">"
                 << (currentTime / 1000) << '.'
                 << std::setw(3) << std::setfill('0')
                 << (currentTime % 1000) 
                 << "</span><span class=\"text\">";
        assert(mLogFile);
    }

    std::ofstream mLogFile;
    bool mPreformatedOutput;
};

// Специализация операторов вывода для различных типов.

template <class T> inline html & html::operator <<( const T & value )
{
    mLogFile << value;
    assert(mLogFile);
    return * this;
}

template <>
inline html & html::operator <<( const logger_::logger_base<logger_::buffer::html> & )
{
    if (!mPreformatedOutput)
    {
        mPreformatedOutput = true;
        mLogFile << "<pre>";
    }
    return *this;
}

template <int N> inline html & html::operator <<( const char (& value)[N] )
{
    return write_escaped_html(value);
}

template <int N> inline html & html::operator <<( const wchar_t (& value)[N] )
{
    return write_escaped_html(utility::wstring2string(value));
}

template <> inline html & html::operator <<( const char * const & value )
{
    return write_escaped_html(value);
}

template <> inline html & html::operator <<( const wchar_t * const & value )
{
    return write_escaped_html(utility::wstring2string(value));
}

template <> inline html & html::operator <<( char * const & value )
{
    return write_escaped_html(value);
}

template <> inline html & html::operator <<( wchar_t * const & value )
{
    return write_escaped_html(utility::wstring2string(value));
}

template <> inline html & html::operator <<( const std::string & value )
{
    return write_escaped_html(value);
}

template <> inline html & html::operator <<( const std::wstring & value )
{
    return write_escaped_html(utility::wstring2string(value));
}

template <> inline html & html::operator <<( const const_string & value )
{
    return write_escaped_html(value.c_str());
}

template <> inline html & html::operator <<( const const_wstring & value )
{
    return write_escaped_html(utility::wstring2string(value.c_str()));
}

template <> inline html & html::operator <<( const std::stringstream & value )
{
    return write_escaped_html(value.str());
}

template <> inline html & html::operator <<( const std::wstringstream & value )
{
    return write_escaped_html(utility::wstring2string(value.str()));
}


}} // namespace logger_::buffer

#endif // _LOGGER_BUFFER_HTML_H_
