/*
 logger
 ������ ���������� ��� ���������������� ������ ���������.
 */
#ifndef _LOGGER_STREAM_H_
#define _LOGGER_STREAM_H_

namespace logger_ { namespace stream
{

//! ������� ����� ��� ���������������� ������� ������.
template<class style>
class stream_base
{
public:
    //! �����, ������������ ��� ������ � �����.
    typedef style style_type;
    
    //! ������� �� ���������.
    enum { is_enabled_by_default = true };

    /**
     ���������� ������, ������������ � ������ �������
     ���������� � ������ ����� ���������.
     */
    static const char * prefix()
    {
        return "";
    }
    
protected:
    // ����������� �������� ������ �����.
    stream_base() {}
};

}} // namespace logger_::stream

#endif // _LOGGER_STREAM_H_
