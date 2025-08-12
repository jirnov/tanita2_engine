///////////////////////////////////////////////////////////////////////////////////////////////
// const_string.hpp

// Copyright (c) 2004 Maxim Yegorushkin
//
// Use, modification and distribution are subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CONST_STRING_CONST_STRING_HPP
#define BOOST_CONST_STRING_CONST_STRING_HPP

#include <utility>
#include <iterator>
#include <string>
#include <stdexcept>

#include "boost/ref.hpp"
#include "boost/type_traits/is_pod.hpp"

#include "boost/const_string/const_string_fwd.hpp"
#include "boost/const_string/detail/storage.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////

namespace boost {

////////////////////////////////////////////////////////////////////////////////////////////////

namespace cs {
namespace aux {

////////////////////////////////////////////////////////////////////////////////////////////////

template<class char_type>
struct zero_string_literal
{
    static char_type const value[1];
};

template<class char_type>
char_type const zero_string_literal<char_type>::value[1] = { 0 };

////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
inline
typename T::const_pointer checked_data(T const& t, typename T::size_type pos)
{
    if(pos > t.size())
        throw std::out_of_range("const_string");
    else
        return pos + t.data();
}

template<class T>
inline
typename T::size_type checked_size(T const& t, typename T::size_type pos, typename T::size_type n)
{
    typename T::size_type const size(t.size());
    if(pos > size)
        throw std::out_of_range("const_string");
    else
        return std::min(n, size - pos);
}

////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace aux {
} // namespace cs {

////////////////////////////////////////////////////////////////////////////////////////////////

template<
      class CharT
    , class TraitsT
    , class StorageT
    >
class const_string : private StorageT
{
private:
    BOOST_STATIC_ASSERT(boost::is_pod<CharT>::value);
    enum { reference_semantics };

public:
    typedef StorageT storage_type;

    typedef CharT value_type;
    typedef CharT char_type;
    typedef TraitsT traits_type;

    typedef size_t size_type;
    typedef char_type const* pointer;
    typedef char_type const* const_pointer;

    typedef char_type const* iterator;
    typedef char_type const* const_iterator;
	typedef std::reverse_iterator<const_iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    static size_t const npos = static_cast<size_t>(-1);

    typedef std::basic_string<char_type, traits_type> std_string_type;

public:
    const_string() // throw(std::bad_alloc) only in case when sizeof(char_type) > sizeof(char_type*) and buffer_size is 0
        : storage_type(
              boost::cs::aux::zero_string_literal<char_type>::value
            , 0
            , reference_semantics
            )
    {}

    explicit const_string(storage_type const& stg) // throw()
        : storage_type(stg)
    {}

public: // const_string arg
    const_string(const_string const& str, size_t pos, size_t n = npos) // throw(std::bad_alloc, std::out_of_range, std::length_error)
        : storage_type(
              cs::aux::checked_data(str, pos)
            , cs::aux::checked_size(str, pos, n)
            )
    {}

    const_string(boost::reference_wrapper<const_string const> const& str) // throw()
        : storage_type(str.get())
    {}

    const_string(boost::reference_wrapper<const_string const> const& str, size_t pos, size_t n = npos) // throw(std::out_of_range, std::length_error)
        : storage_type(
              cs::aux::checked_data(str.get(), pos)
            , cs::aux::checked_size(str.get(), pos, n)
            , reference_semantics
            )
    {}

    const_string& operator=(const_string const& str) // throw()
    {
		this->storage_type::operator=(str);
		return *this;
	}

	const_string& operator=(boost::reference_wrapper<const_string const> const& str) // throw()
    {
		this->storage_type::operator=(str.get());
		return *this;
	}

public: // std::basic_string<> arg
    const_string(std_string_type const& str, size_t pos = 0, size_t n = npos) // throw(std::bad_alloc, std::out_of_range, std::length_error)
        : storage_type(
              cs::aux::checked_data(str, pos)
            , cs::aux::checked_size(str, pos, n)
            )
    {}

    const_string(boost::reference_wrapper<std_string_type const> const& str, size_t pos = 0, size_t n = npos) // throw(std::out_of_range, std::length_error)
        : storage_type(
              cs::aux::checked_data(str.get(), pos)
            , cs::aux::checked_size(str.get(), pos, n)
            , reference_semantics
            )
    {}

public: // char_type* arg
    const_string(char_type const* s, size_t n = npos) // throw(std::bad_alloc, std::length_error)
        : storage_type(
              s
            , npos == n ? traits_type::length(s) : n
        )
    {}

    const_string(boost::reference_wrapper<char_type const* const> s, size_t n = npos) // throw(std::length_error)
        : storage_type(
              s.get()
            , npos == n ? traits_type::length(s.get()) : n
            , reference_semantics
        )
    {}

    const_string(boost::reference_wrapper<char_type* const> s, size_t n = npos) // throw(std::length_error)
        : storage_type(
              s.get()
            , npos == n ? traits_type::length(s.get()) : n
            , reference_semantics
        )
    {}

    const_string(char_type const* begin, char_type const* end) // throw(std::bad_alloc, std::length_error)
        : storage_type(
              begin
            , end - begin
        )
    {}

    const_string(boost::reference_wrapper<char_type const* const> begin, char_type const* end) // throw(std::length_error)
        : storage_type(
              begin.get()
			, end - begin.get()
            , reference_semantics
        )
    {}

    const_string(boost::reference_wrapper<char_type* const> begin, char_type const* end) // throw(std::length_error)
        : storage_type(
              begin.get()
			, end - begin.get()
            , reference_semantics
        )
    {}

public: // char_type[] arg
    template<size_t N>
    const_string(boost::reference_wrapper<char_type const[N]> const& s, size_t n = npos) // throw(std::length_error)
        : storage_type(
              s.get()
            , npos == n ? traits_type::length(s.get()) : n
            , reference_semantics
        )
    {}

public: // other
    template<class IteratorT>
    const_string(IteratorT begin, IteratorT end) // throw(std::bad_alloc, std::length_error)
        : storage_type(0, std::distance(begin, end))
    {
        std::copy(begin, end, const_cast<char_type*>(this->data()));
    }

    const_string(size_t n, char_type c) // throw(std::bad_alloc, std::length_error)
        : storage_type(0, n)
    {
        std::fill_n(const_cast<char_type*>(this->begin()), n, c);
    }

    const_string& operator=(char_type c) // throw(std::bad_alloc)
    {
        return *this = const_string(1, c);
    }


public:
    template<class T, class U>
    const_string(concatenation<const_string, T, U> const& expr) // throw(std::bad_alloc, std::length_error)
        : storage_type(0, expr.size())
    {
        expr.copy_result_to(const_cast<char_type*>(this->begin()));
    }

public:
    const_iterator begin() const { return this->storage_type::begin(); } // throw()
    const_iterator end() const { return this->storage_type::end(); } // throw()
    const_reverse_iterator rbegin() const { return reverse_iterator(this->end()); } // throw()
    const_reverse_iterator rend() const { return reverse_iterator(this->begin()); } // throw()

    size_t max_size() const { return this->storage_type::max_size(); } // throw() 
    size_t size() const { return this->storage_type::size(); } // throw()
    size_t length() const { return this->storage_type::size(); } // throw()
    bool empty() const { return 0 == this->storage_type::size(); } // throw()

    char_type operator[](size_t index) const // throw()
    {
        return index < this->size()
            ? this->begin()[index]
            : char_type()
            ;
    }

    char_type at(size_t index) const // throw(std::out_of_range)
    {
        if(index < this->size())
            return (*this)[index];
        else
            throw std::out_of_range("invalid index");
    }

    char_type const* data() const // throw(), may not have the trailing zero
    {
        return this->begin();
    }

    char_type const* c_str() const // throw(std::bad_alloc), always has the trailing zero
    {
        // this condition yields true if this string was obtained via ref_substr()
        // and thus may not have the trailing zero
        if(char_type() != *this->end())
            const_cast<const_string&>(*this) = const_string(this->begin(), this->end());
        return this->begin();
    }

    size_t copy(char_type* s, size_t n, size_t pos = 0) const
    {
        size_t const size(this->size());

        if(pos > size)
            throw std::out_of_range("copy");

        n = std::min(n, size - pos);
        traits_type::copy(s, this->begin() + pos, n);

        return n;
    }

    std_string_type str() const // throw(std::bad_alloc)
    {
        return std_string_type(this->begin(), this->end());
    }

    const_string substr(size_t pos = 0, size_t n = npos) const // throw(std::bad_alloc, std::out_of_range)
    {
        return const_string(*this, pos, n);
    }

    const_string ref_substr(size_t pos = 0, size_t n = npos) const // throw(std::out_of_range, std::length_error)
    {
        return const_string(boost::cref(*this), pos, n);
    }

public:
    void clear() // throw()
    {
        *this = const_string();
    }

    void swap(const_string& other) // throw()
    {
        const_string const t(*this);
        *this = other;
        other = t;
    }
    
public:
    // append(), push_back() and operator+=() are inefficient
    // provided for drop in compatibility with std::basic_string<> only
    // #include "boost/const_string/concatenation.hpp" to use them

    const_string& operator+=(const_string const& str) // throw(std::bad_alloc, std::length_error)
    {
        return *this = *this + str;
    }

    const_string& operator+=(std_string_type const& str) // throw(std::bad_alloc, std::length_error)
    {
        return *this = *this + str;
    }

    const_string& operator+=(char_type const* s) // throw(std::bad_alloc, std::length_error)
    {
        return *this = *this + s;
    }

    const_string& operator+=(char_type c) // throw(std::bad_alloc, std::length_error)
    {
        return *this = *this + const_string(1, c);
    }

    const_string& append(const_string const& str) // throw(std::bad_alloc, std::length_error)
    {
        return *this = *this + str;
    }

    const_string& append(const_string const& str, size_t pos, size_t n) // throw(std::bad_alloc, std::length_error)
    {
        return *this = *this + str.ref_substr(pos, n);
    }

    const_string& append(std_string_type const& str) // throw(std::bad_alloc, std::length_error)
    {
        return *this = *this + str;
    }

    const_string& append(std_string_type const& str, size_t pos, size_t n) // throw(std::bad_alloc, std::length_error)
    {
        char_type const* const data(str.data());
        return *this = *this + const_string(boost::cref(data), pos, n);
    }

    const_string& append(char_type const* s) // throw(std::bad_alloc, std::length_error)
    {
        return *this = *this + s;
    }

    const_string& append(char_type const* s, size_t n) // throw(std::bad_alloc, std::length_error)
    {
        return *this = *this + const_string(boost::cref(s), n);
    }

    const_string& append(char_type c, size_t n) // throw(std::bad_alloc, std::length_error)
    {
        return *this = *this + const_string(n, c);
    }

    const_string& append(char_type const* begin, char_type const* end) // throw(std::bad_alloc, std::length_error)
    {
        return *this = *this + const_string(boost::cref(begin), end);
    }

    template<class IteratorT>
    const_string& append(IteratorT begin, IteratorT end) // throw(std::bad_alloc, std::length_error)
    {
        return *this = *this + const_string(begin, end);
    }

    void push_back(char_type c) // throw(std::bad_alloc, std::length_error)
    {
        this->append(c, 1);
    }

public: // assign
    const_string& assign(const_string const& str) // throw(std::bad_alloc, std::length_error)
    {
        return *this = str;
    }

    const_string& assign(const_string const& str, size_t pos, size_t n) // throw(std::bad_alloc, std::length_error)
    {
        return *this = str.ref_substr(pos, n);
    }

    const_string& assign(std_string_type const& str) // throw(std::bad_alloc, std::length_error)
    {
        return *this = str;
    }

    const_string& assign(std_string_type const& str, size_t pos, size_t n) // throw(std::bad_alloc, std::length_error)
    {
        return *this = const_string(str, pos, n);
    }

    const_string& assign(char_type const* s) // throw(std::bad_alloc, std::length_error)
    {
        return *this = s;
    }

    const_string& assign(char_type const* s, size_t n) // throw(std::bad_alloc, std::length_error)
    {
        return *this = const_string(s, n);
    }

    const_string& assign(char_type c, size_t n) // throw(std::bad_alloc, std::length_error)
    {
        return *this = const_string(n, c);
    }

    template<class IteratorT>
    const_string& assign(IteratorT begin, IteratorT end) // throw(std::bad_alloc, std::length_error)
    {
        return *this = const_string(begin, end);
    }

public: // find
    size_t find(const_string const& str, size_t pos = 0) const // throw()
    {
        size_t const this_size(this->size());
        size_t const str_size(str.size());
        char_type const* this_data(this->data());
        char_type const* str_data(str.data());

        for (; pos + str_size <= this_size; ++pos)
            if (!traits_type::compare(this_data + pos, str_data, str_size))
                return pos;

        return npos;
    }

    size_t rfind(const_string const& str, size_t pos = 0) const // throw()
    {
        size_t const this_size(this->size());
        size_t const str_size(str.size());
        char_type const* this_data(this->data());
        char_type const* str_data(str.data());

        if (str_size <= this_size)
        {
            pos = std::min(this_size - str_size, pos);
            do 
            {
                if (!traits_type::compare(this_data + pos, str_data, str_size))
                    return pos;
            } 
            while (pos-- > 0);
        }
        return npos;
    }

    size_t find_first_of(char_type const* s, size_type pos, size_type n) const // throw()
    {
        size_t const size(this->size());
        for (; n && pos < size; ++pos)
        {
            if(traits_type::find(s, n, (*this)[pos]))
                return pos;
        }
        return npos;
    }

    size_t find_last_of(char_type const* s, size_type pos, size_type n) const // throw()
    {
        size_type size(this->size());
        if(size && n)
        { 
            if(--size > pos) 
                size = pos;
            do
            {
                if(traits_type::find(s, n, (*this)[size]))
                    return size;
            } 
            while(size-- != 0);
        }
        return npos;
    }

    size_t find_first_not_of(char_type const* s, size_type pos, size_type n) const // throw()
    {
        size_t const size(this->size());
        for(size_t xpos(pos); xpos < size; ++xpos)
            if(!traits_type::find(s, n, (*this)[xpos]))
                return xpos;
        return npos;
    }

    size_t find_first_not_of(char_type c, size_type pos = npos) const // throw()
    {
        size_t const size(this->size());
        for(size_t xpos(pos); xpos < size; ++xpos)
            if(!traits_type::eq((*this)[xpos], c))
                return xpos;
        return npos;
    }

    size_t find_last_not_of(char_type const* s, size_type pos, size_type n) const // throw()
    {
        size_t size(this->size());
        if(size)
        { 
            if(--size > pos) 
                size = pos;
            do
            {
                if(!traits_type::find(s, n, (*this)[size]))
                    return size;
            } 
            while(size--);
        }
        return npos;
    }

    size_t find_last_not_of(char_type c, size_type pos = npos) const // throw()
    {
        size_t size(this->size());
        if(size)
        { 
            if(--size > pos) 
                size = pos;
            do
            {
                if(!traits_type::eq((*this)[size], c))
                    return size;
            } 
            while(size--);
        }
        return npos;
    }

    size_t find(char_type const* s, size_t pos, size_t n) const // throw(std::length_error)
    {
        return this->find(const_string(boost::cref(s), n), pos);
    }

    size_t find(char_type const* s, size_t pos = 0) const // throw(std::length_error)
    {
        return this->find(const_string(boost::cref(s)), pos);
    }

    size_t find(char_type s, size_t pos = 0) const // throw()
    {
        return this->find(const_string(boost::cref(&s), 1), pos);
    }

    size_t rfind(char_type const* s, size_t pos, size_t n) const // throw(std::length_error)
    {
        return this->rfind(const_string(boost::cref(s), n), pos);
    }

    size_t rfind(char_type const* s, size_t pos = 0) const // throw(std::length_error)
    {
        return this->rfind(const_string(boost::cref(s), pos));
    }

    size_t rfind(char_type s, size_t pos = 0) const // throw()
    {
        return this->rfind(const_string(boost::cref(&s), 1), pos);
    }

    size_t find_first_of(const_string const& str, size_t pos = 0) const // throw()
    {
        return this->find_first_of(str.data(), pos, str.size());
    }

    size_t find_first_of(char_type const* s, size_t pos = 0) const // throw()
    {
        return this->find_first_of(s, pos, traits_type::length(s));
    }

    size_t find_first_of(char_type c, size_t pos = 0) const // throw()
    {
        return this->find(c, pos);
    }

    size_t find_last_of(const_string const& str, size_t pos = npos) const // throw()
    {
        return this->find_last_of(str.data(), pos, str.size());
    }

    size_t find_last_of(char_type const* s, size_t pos = npos) const // throw()
    {
        return this->find_last_of(s, pos, traits_type::length(s));
    }

    size_t find_last_of(char_type c, size_t pos = npos) const // throw()
    {
        return this->rfind(c, pos);
    }

    size_t find_first_not_of(const_string const& str, size_t pos = 0) const // throw()
    {
        return this->find_first_not_of(str.data(), pos, str.size());
    }

    size_t find_first_not_of(char_type const* s, size_t pos = 0) const // throw()
    {
        return this->find_first_not_of(s, pos, traits_type::length(s));
    }

    size_t find_last_not_of(const_string const& str, size_t pos = npos) const // throw()
    {
        return this->find_last_not_of(str.data(), pos, str.size());
    }

    size_t find_last_not_of(char_type const* s, size_t pos = npos) const // throw()
    {
        return this->find_last_not_of(s, pos, traits_type::length(s));
    }

public: // compare
    template<class S>
    int compare(const_string<char_type, traits_type, S> const& b) const // throw()
    {
        size_t const a_lenght(this->size());
        size_t const b_lenght(b.size());
        int const res(traits_type::compare(this->data(), b.data(), std::min(a_lenght, b_lenght)));
        return res ? res : static_cast<int>(a_lenght - b_lenght);
    }

    template<class S>
    int compare(
          size_t pos1
        , size_t n1
        , const_string<char_type, traits_type, S> const& str
        ) const // throw(std::out_of_range, std::length_error)
    {
        return this->ref_substr(pos1, n1).compare(str);
    }

    template<class S>
    int compare(
          size_t pos1
        , size_t n1
        , const_string<char_type, traits_type, S> const& str
        , size_t pos2
        , size_t n2) const // throw(std::out_of_range, std::length_error)
    {
        return this->ref_substr(pos1, n1).compare(str.ref_substr(pos2, n2));
    }

    int compare(char_type const* s) const // throw(std::length_error)
    {
        return this->compare(const_string(boost::cref(s)));
    }

    int compare(
          size_t pos1
        , size_t n1
        , char_type const* s
        , size_t n2 = npos) const // throw(std::length_error)
    {
        return this->ref_substr(pos1, n1).compare(const_string(boost::cref(s), n2));
    }
};

template<class T1, class T2, class T3>
size_t const boost::const_string<T1, T2, T3>::npos;

////////////////////////////////////////////////////////////////////////////////////////////////

template<class CharT, size_t N>
inline const_string<CharT> lit(CharT const(&literal)[N]) // throw()
{
    return const_string<CharT>(boost::cref(literal), N - 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////

#define CONST_STRING_DEFINE_COMPARISON(op) \
template<class char_type, class traits_type, class S1, class S2> \
inline \
bool operator op( \
      const_string<char_type, traits_type, S1> const& a \
    , const_string<char_type, traits_type, S2> const& b \
    ) \
{ \
    return a.compare(b) op 0; \
}

CONST_STRING_DEFINE_COMPARISON(==)
CONST_STRING_DEFINE_COMPARISON(!=)
CONST_STRING_DEFINE_COMPARISON(<)
CONST_STRING_DEFINE_COMPARISON(<=)
CONST_STRING_DEFINE_COMPARISON(>)
CONST_STRING_DEFINE_COMPARISON(>=)

#undef CONST_STRING_DEFINE_COMPARISON

////////////////////////////////////////////////////////////////////////////////////////////////

#define CONST_STRING_DEFINE_COMPARISONS \
CONST_STRING_DEFINE_COMPARISON(==) \
CONST_STRING_DEFINE_COMPARISON(!=) \
CONST_STRING_DEFINE_COMPARISON(<) \
CONST_STRING_DEFINE_COMPARISON(>) \
CONST_STRING_DEFINE_COMPARISON(<=) \
CONST_STRING_DEFINE_COMPARISON(>=)

#define CONST_STRING_DEFINE_COMPARISON(cmp) \
CONST_STRING_DEFINE_COMPARISON_A(cmp) \
CONST_STRING_DEFINE_COMPARISON_B(cmp)

#define CONST_STRING_DEFINE_COMPARISON_A(cmp) \
template<class T1, class T2, class T3 CONST_STRING_CMP_TMPL_AGRS> \
inline bool operator cmp(const_string<T1, T2, T3> const& a, CONST_STRING_CMP_ARG b) \
{ \
    typedef const_string<T1, T2, T3> string; \
    return a cmp string(boost::cref(b)); \
}

#define CONST_STRING_DEFINE_COMPARISON_B(cmp) \
template<class T1, class T2, class T3 CONST_STRING_CMP_TMPL_AGRS> \
inline bool operator cmp(CONST_STRING_CMP_ARG a, const_string<T1, T2, T3> const& b) \
{ \
    typedef const_string<T1, T2, T3> string; \
    return string(boost::cref(a)) cmp b; \
}

#define CONST_STRING_CMP_TMPL_AGRS , class U2, class U3
#define CONST_STRING_CMP_ARG std::basic_string<T1, U2, U3> const&
CONST_STRING_DEFINE_COMPARISONS
#undef CONST_STRING_CMP_ARG
#undef CONST_STRING_CMP_TMPL_AGRS

#define CONST_STRING_CMP_TMPL_AGRS 
#define CONST_STRING_CMP_ARG T1 const*
CONST_STRING_DEFINE_COMPARISONS
#undef CONST_STRING_CMP_ARG
#undef CONST_STRING_CMP_TMPL_AGRS

#undef CONST_STRING_DEFINE_COMPARISON_B
#undef CONST_STRING_DEFINE_COMPARISON_A
#undef CONST_STRING_DEFINE_COMPARISON
#undef CONST_STRING_DEFINE_COMPARISONS

////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace boost

////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BOOST_CONST_STRING_CONST_STRING_HPP

////////////////////////////////////////////////////////////////////////////////////////////////

