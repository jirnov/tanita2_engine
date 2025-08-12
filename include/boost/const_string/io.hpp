////////////////////////////////////////////////////////////////////////////////////////////////
// io.hpp

// Copyright (c) 2004 Maxim Yegorushkin
//
// Use, modification and distribution are subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CONST_STRING_IO_HPP
#define BOOST_CONST_STRING_IO_HPP

#include <iostream>

#include "boost/const_string/const_string_fwd.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////

namespace boost {

////////////////////////////////////////////////////////////////////////////////////////////////

template<class char_type, class traits_type, class T3>
std::basic_ostream<char_type, traits_type>& operator<<(
	  std::basic_ostream<char_type, traits_type>& o
	, const_string<char_type, traits_type, T3> const& s
	)
{
    return o << std::basic_string<char_type, traits_type>(s.data(), s.size());
}

template<class char_type, class traits_type, class T3>
std::basic_istream<char_type, traits_type>& operator>>(
	  std::basic_istream<char_type, traits_type>& i
	, const_string<char_type, traits_type, T3>& s
	)
{
    std::basic_string<char_type, traits_type> t;
	i >> t;
	s = t;
	return i;
}

////////////////////////////////////////////////////////////////////////////////////////////////

template<class char_type, class traits_type, class T3>
std::basic_istream<char_type, traits_type>& getline(
	  std::basic_istream<char_type, traits_type>& i
	, const_string<char_type, traits_type, T3>& s
	)
{
    std::basic_string<char_type, traits_type> t;
    getline(i, t);
    s = t;
    return i;
}

template<class char_type, class traits_type, class T3>
std::basic_istream<char_type, traits_type>& getline(
	  std::basic_istream<char_type, traits_type>& i
	, const_string<char_type, traits_type, T3>& s
    , char_type delim
	)
{
    std::basic_string<char_type, traits_type> t;
    getline(i, t, delim);
    s = t;
    return i;
}

////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace boost {

////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BOOST_CONST_STRING_IO_HPP

////////////////////////////////////////////////////////////////////////////////////////////////

