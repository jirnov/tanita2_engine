////////////////////////////////////////////////////////////////////////////////////////////////
// concatenation.hpp

// Copyright (c) 2004 Maxim Yegorushkin
//
// Use, modification and distribution are subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CONST_STRING_CONCATENATION_HPP
#define BOOST_CONST_STRING_CONCATENATION_HPP

#include <string>

#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_same.hpp"

#include "boost/const_string/const_string_fwd.hpp"

#ifdef BOOST_MSVC
#   pragma warning(push)
#   pragma warning(disable: 4512)
#endif // BOOST_MSVC

////////////////////////////////////////////////////////////////////////////////////////////////

namespace boost {

////////////////////////////////////////////////////////////////////////////////////////////////

template<class StringT, class LeftT, class RightT>
class concatenation
{
public:
    concatenation(LeftT const& a, RightT const& b)
        : left_(a)
        , right_(b)
        , size_(static_cast<size_t>(-1))
    {}

public:
    StringT str() const { return StringT(*this); }
    size_t size() const { return static_cast<size_t>(-1) == size_ ? size_ = left_.size() + right_.size() : size_; }
    void copy_result_to(typename StringT::char_type*) const;

private:
    LeftT const left_;
    RightT const right_;
    mutable size_t size_;
};

////////////////////////////////////////////////////////////////////////////////////////////////

namespace cs { namespace aux {

////////////////////////////////////////////////////////////////////////////////////////////////

template<class StringT, class T>
struct expression_arg
{
    typedef StringT type;
};

template<class StringT, class T, class U>
struct expression_arg<StringT, concatenation<StringT, T, U> >
{
    typedef concatenation<StringT, T, U> type;
};

template<class StringT, class T, class U>
struct expression_result
{
    typedef concatenation<
          StringT
        , typename expression_arg<StringT, T>::type
        , typename expression_arg<StringT, U>::type
        > type;
};

template<class StringT>
struct wrap_expression_arg
{
    static StringT const& wrap(StringT const& t)
    {
        return t;
    }

    template<class T>
    static StringT wrap(T const& t)
    {
        return StringT(boost::cref(t));
    }

    template<class T, class U>
    static concatenation<StringT, T, U> const& wrap(concatenation<StringT, T, U> const& t)
    {
        return t;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////

}} // namespace cs { namespace aux {

////////////////////////////////////////////////////////////////////////////////////////////////

template<
      class T1
    , class T2
    , class T3
    >
inline
typename cs::aux::expression_result<
      const_string<T1, T2, T3>
    , const_string<T1, T2, T3>
    , const_string<T1, T2, T3>
    >::type
operator+(const_string<T1, T2, T3> const& a, const_string<T1, T2, T3> const& b) // throw()
{
    typedef const_string<T1, T2, T3> StringT;
    typedef typename cs::aux::expression_result<StringT, StringT, StringT>::type type;
    typedef cs::aux::wrap_expression_arg<StringT> wrap_t;
    return type(wrap_t::wrap(a), wrap_t::wrap(b));
}

template<
      class T1
    , class T2
    , class T3
    , class U
    >
inline
typename cs::aux::expression_result<
      const_string<T1, T2, T3>
    , const_string<T1, T2, T3>
    , U
    >::type
operator+(const_string<T1, T2, T3> const& a, U const& b) // throw()
{
    typedef const_string<T1, T2, T3> StringT;
    typedef typename cs::aux::expression_result<StringT, StringT, U>::type type;
    typedef cs::aux::wrap_expression_arg<StringT> wrap_t;
    return type(wrap_t::wrap(a), wrap_t::wrap(b));
}

template<
      class T1
    , class T2
    , class T3
    , class U
    >
inline
typename cs::aux::expression_result<
      const_string<T1, T2, T3>
    , U
    , const_string<T1, T2, T3>
    >::type
operator+(U const& a, const_string<T1, T2, T3> const& b) // throw()
{
    typedef const_string<T1, T2, T3> StringT;
    typedef typename cs::aux::expression_result<StringT, U, StringT>::type type;
    typedef cs::aux::wrap_expression_arg<StringT> wrap_t;
    return type(wrap_t::wrap(a), wrap_t::wrap(b));
}

template<
      class T1
    , class T2
    , class T3
    , class U
    , class V1
    , class V2
    >
inline
typename boost::lazy_disable_if<
      boost::is_same<const_string<T1, T2, T3>, U>
    , typename cs::aux::expression_result<
          const_string<T1, T2, T3>
        , concatenation<const_string<T1, T2, T3>, V1, V2>
        , U
        >
    >::type
operator+(concatenation<const_string<T1, T2, T3>, V1, V2> const& a, U const& b) // throw()
{
    typedef const_string<T1, T2, T3> StringT;
    typedef typename cs::aux::expression_result<
          StringT
        , concatenation<StringT, V1, V2>
        , U
        >::type type;
    typedef cs::aux::wrap_expression_arg<StringT> wrap_t;
    return type(wrap_t::wrap(a), wrap_t::wrap(b));
}

template<
      class T1
    , class T2
    , class T3
    , class U
    , class V1
    , class V2
    >
inline
typename boost::lazy_disable_if<
      boost::is_same<const_string<T1, T2, T3>, U>
    , typename cs::aux::expression_result<
          const_string<T1, T2, T3>
        , U
        , concatenation<const_string<T1, T2, T3>, V1, V2>
        >
    >::type
operator+(U const& a, concatenation<const_string<T1, T2, T3>, V1, V2> const& b) // throw()
{
    typedef const_string<T1, T2, T3> StringT;
    typedef typename cs::aux::expression_result<
          StringT
        , U
        , concatenation<StringT, V1, V2>
        >::type type;
    typedef cs::aux::wrap_expression_arg<StringT> wrap_t;
    return type(wrap_t::wrap(a), wrap_t::wrap(b));
}

////////////////////////////////////////////////////////////////////////////////////////////////

namespace cs { namespace aux {

////////////////////////////////////////////////////////////////////////////////////////////////

template<
      class char_type
    , class traits_type
    , class S
    >
inline
void copy_result_to(
      char_type* to
    , boost::const_string<char_type, traits_type, S> const& from
    )
{
    traits_type::copy(to, from.data(), from.size());
}

template<
      class char_type
    , class traits_type
    , class storage_t
    , class V1
    , class V2
    >
inline
void copy_result_to(
      char_type* to
    , boost::concatenation<boost::const_string<char_type, traits_type, storage_t>, V1, V2> const& from
    )
{
    from.copy_result_to(to);
}

////////////////////////////////////////////////////////////////////////////////////////////////

}}} // namespace boost { namespace cs { namespace aux {

////////////////////////////////////////////////////////////////////////////////////////////////

template<class StringT, class LeftT, class RightT>
inline
void boost::concatenation<StringT, LeftT, RightT>::copy_result_to(typename StringT::char_type* to) const
{
    cs::aux::copy_result_to(to, left_);
    cs::aux::copy_result_to(to + left_.size(), right_);
}

////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef BOOST_MSVC
#   pragma warning(pop)
#endif // BOOST_MSVC

////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BOOST_CONST_STRING_CONCATENATION_HPP

////////////////////////////////////////////////////////////////////////////////////////////////

