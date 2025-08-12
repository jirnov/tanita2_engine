/*
 logger
 ������ ���������� ��� ���������������� ������ ���������.
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
    //! ����������� ��� ���������� �������������� ������ �� ����� ������.
    struct pre_manupulator
    {
        inline pre_manupulator & operator()() const
        {
            return *this;
        }
    };
    
    
    //! ����������� ����� ������ ��� ������ � �����.
    /**
     ����� ��� ��������� ��������-������� ������, ������������
     �������� user_stream ��� ������ � �����.
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
        // ����� �� ����� ���� �������/�������� � �������� ������ � ����.
        internal_stream( buffer_type & buffer, const char * prefix, bool isEnabled, TIME currentTime )
            : mBuffer(buffer), mIsEnabled(isEnabled)
        {
            if (isEnabled)
            {
                mBuffer.begin(style(), currentTime);
                mBuffer << prefix;
            }
        }
        
        // �������� ������������ ��������.
        internal_stream & operator =( const internal_stream & );

        buffer_type & mBuffer;
        const bool mIsEnabled;

        friend class logger_base;
    };

    //! ������� ����� ��� ������ ��������� ��� ������� ��� ��������������
    logger_base()
        : mBuffer(), mStartTime(getCurrentTime())
    {
    }

    //! ������� ����� ��� ������ ���������
    explicit logger_base( const typename buffer_type::initializer & initializer )
        : mBuffer(initializer), mStartTime(getCurrentTime())
    {
    }

    //! ���������� ������ ��� ������ � �������� �����.
    template <class stream>
    internal_stream<typename stream::style_type> user_stream() const
    {
        return internal_stream<typename stream::style_type>(mBuffer, stream::prefix(),
                                                            stream_<stream>::smIsEnabled,
                                                            getCurrentTime() - mStartTime);
    }

    //! ��������/��������� ������ � �������� �����.
    template <class stream>
    void enable_stream( bool is_enabled )
    {
        stream_<stream>::smIsEnabled = is_enabled;
    }
    
    //! �������� �� ��������� ������ � �����.
    template <class stream>
    bool is_stream_enabled() const
    {
        return stream_<stream>::smIsEnabled;
    }

private:
    // ���-������� ��� �������� ����� ���������� ������.
    template<class stream>
    struct stream_ : stream
    {
        static bool smIsEnabled;
    };
    
    // ����� ������.
    mutable buffer_type mBuffer;
    
    // ����� � ������� �������� �������.
    const TIME mStartTime;
};

template <class buffer_type>
template<class stream>
bool logger_base<buffer_type>::stream_<stream>::smIsEnabled = stream::is_enabled_by_default;

} // namespace logger_

#endif // _LOGGER_LOGGER_H_
