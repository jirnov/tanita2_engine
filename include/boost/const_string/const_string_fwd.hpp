///////////////////////////////////////////////////////////////////////////////////////////////
// const_string_fwd.hpp

// Copyright (c) 2004 Maxim Yegorushkin
//
// Use, modification and distribution are subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CONST_STRING_CONST_STRING_FWD_HPP
#define BOOST_CONST_STRING_CONST_STRING_FWD_HPP

///////////////////////////////////////////////////////////////////////////////////////////////

namespace boost {

///////////////////////////////////////////////////////////////////////////////////////////////

template<
      class TraitsT
    , class AllocatorT = std::allocator<typename TraitsT::char_type>
    , size_t buffer_size = (16 - sizeof(size_t)) / sizeof(typename TraitsT::char_type)
    , size_t buffer_alignment = 0
    >
class const_string_storage;

template<
      class CharT
    , class TraitsT = std::char_traits<CharT>
    , class StorageT = const_string_storage<TraitsT>
    >
class const_string;

template<class CharT, class LeftT, class RightT>
class concatenation;

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace boost

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // BOOST_CONST_STRING_CONST_STRING_FWD_HPP

///////////////////////////////////////////////////////////////////////////////////////////////

