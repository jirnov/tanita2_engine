////////////////////////////////////////////////////////////////////////////////////////////////
// format.hpp

// Copyright (c) 2004 Maxim Yegorushkin
//
// Use, modification and distribution are subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CONST_STRING_FORMAT_HPP
#define BOOST_CONST_STRING_FORMAT_HPP

#include <stdarg.h>

#include "boost/const_string/const_string.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////

namespace boost {

////////////////////////////////////////////////////////////////////////////////////////////////

namespace cs {
namespace aux {

inline int do_format(char* s, size_t n, char const* fmt, va_list args)
{
#if defined(BOOST_MSVC) || defined(BOOST_INTEL_WIN)
    return ::_vsnprintf(s, n, fmt, args);
#else
    return ::vsnprintf(s, n, fmt, args);
#endif
}

#ifndef BOOST_NO_CWCHAR

inline int do_format(wchar_t* s, size_t n, wchar_t const* fmt, va_list args)
{
#if defined(BOOST_MSVC) || defined(BOOST_INTEL_WIN)
    return ::_vsnwprintf(s, n, fmt, args);
//#elif defined(__MWERKS__) || defined(__GNUC__)
#else
    return ::vswprintf(s, n, fmt, args);
//#else
//    return ::vsnwprintf(s, n, fmt, args);
#endif
}

#endif // BOOST_NO_CWCHAR

////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace aux {
} // namespace cs {

////////////////////////////////////////////////////////////////////////////////////////////////

template<class ConstStringT, class CharT> // MSVC chokes without CharT parameter
inline ConstStringT cs_format_va(size_t hint, CharT const* fmt, va_list args)
{
    typedef ConstStringT string;
    typedef typename string::char_type char_type;
    typedef typename string::storage_type storage_type;

    if(!hint)
        hint = string::traits_type::length(fmt);
    ++hint; // include the trailing zero

    // first take - try not to allocate any memory
    enum { buffers_chars = storage_type::effective_buffer_size_chars };
    if(buffers_chars && hint <= buffers_chars)
    {
        storage_type stg(0, buffers_chars - 1);
        int const r(cs::aux::do_format(const_cast<char_type*>(stg.begin()), buffers_chars, fmt, args));
        if(size_t(r) < buffers_chars)
            return string(stg.set_size(r));
        else
            hint = 2 * buffers_chars;
    }

    // grow buffer till it fits
    for(; true; hint *= 2)
    {
        storage_type stg(0, hint - 1);
        int const r(cs::aux::do_format(const_cast<char_type*>(stg.begin()), hint, fmt, args));
        if(size_t(r) < hint)
            return string(stg.set_size(r));
    }
    // never get here
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline const_string<char> cs_format(char const* fmt, ...)
{
    typedef const_string<char> string;
    va_list args; va_start(args, fmt);
    string r = cs_format_va<string, string::char_type>(0, fmt, args);
    va_end(args);
    return r;
}

inline const_string<char> cs_format(size_t hint, char const* fmt, ...)
{
    typedef const_string<char> string;
    va_list args; va_start(args, fmt);
    string r = cs_format_va<string, string::char_type>(hint, fmt, args);
    va_end(args);
    return r;
}

#ifndef BOOST_NO_CWCHAR

inline const_string<wchar_t> cs_format(wchar_t const* fmt, ...)
{
    typedef const_string<wchar_t> string;
    va_list args; va_start(args, fmt);
    string const& r(cs_format_va<string, string::char_type>(0, fmt, args));
    va_end(args);
    return r;
}

inline const_string<wchar_t> cs_format(size_t hint, wchar_t const* fmt, ...)
{
    typedef const_string<wchar_t> string;
    va_list args; va_start(args, fmt);
    string const& r(cs_format_va<string, string::char_type>(hint, fmt, args));
    va_end(args);
    return r;
}

#endif // BOOST_NO_CWCHAR

template<class ConstStringT>
inline ConstStringT cs_format(typename ConstStringT::char_type const* fmt, ...)
{
    typedef ConstStringT string;
    va_list args; va_start(args, fmt);
    string const& r(cs_format_va<string, typename string::char_type>(0, fmt, args));
    va_end(args);
    return r;
}

template<class ConstStringT>
inline ConstStringT cs_format(size_t hint, typename ConstStringT::char_type const* fmt, ...)
{
    typedef ConstStringT string;
    va_list args; va_start(args, fmt);
    string const& r(cs_format_va<string, typename string::char_type>(hint, fmt, args));
    va_end(args);
    return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace boost

////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BOOST_CONST_STRING_FORMAT_HPP

////////////////////////////////////////////////////////////////////////////////////////////////
