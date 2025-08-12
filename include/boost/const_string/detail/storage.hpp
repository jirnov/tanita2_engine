////////////////////////////////////////////////////////////////////////////////////////////////
// storage.hpp

// Copyright (c) 2004 Maxim Yegorushkin
//
// Use, modification and distribution are subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CONST_STRING_DETAIL_STORAGE_HPP
#define BOOST_CONST_STRING_DETAIL_STORAGE_HPP

#include <limits>
#include <new>
#include <memory>

#include "boost/aligned_storage.hpp"
#include "boost/detail/atomic_count.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////

namespace boost
{

////////////////////////////////////////////////////////////////////////////////////////////////

namespace cs {
namespace aux {

// Ben Hutchings reported:
//
// There's one possible problem I noticed, which is that an allocated
// buffer will be aligned properly for boost::detail::atomic_count but
// perhaps might not be aligned properly for char_type (or to whatever
// buffer_alignment specifies).  Let me apply the James Kanze
// test [*]: is it safe to create a const_string<double> on a SPARC?
//  
// [*] See <http://gcc.gnu.org/bugzilla/show_bug.cgi?id=8670> if you
//     don't know what I'm talking about.
//
// This is the fix.
//
template<class T, class U>
struct aligned_union
{
    union type
    {
        typename boost::type_with_alignment<
              (boost::alignment_of<T>::value > boost::alignment_of<U>::value)
            ? boost::alignment_of<T>::value 
            : boost::alignment_of<U>::value
            >::type a;
        char b[sizeof(T)];
        char c[sizeof(U)];
    };
};

}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// The class is a storage strategy.
//
// logic:
//     if the source string should be referenced (boost::cref)
//         then it stores a pointer to the source string
//     else if the source string size including trailing zero is no greater than effective_buffer_size
//         then a copy of the source sting is stored in the buffer inside the string
//     else
//         it allocates and shares reference counted copy of the string

template<
      class TraitsT
    , class AllocatorT
    , size_t buffer_size
    , size_t buffer_alignment
    >
class const_string_storage 
    : private AllocatorT::template rebind<
          typename cs::aux::aligned_union<boost::detail::atomic_count, typename TraitsT::char_type>::type
      >::other
{
private:
    typedef TraitsT traits_type;
    typedef typename TraitsT::char_type char_type;
    typedef typename AllocatorT::template rebind<
        typename cs::aux::aligned_union<boost::detail::atomic_count, typename TraitsT::char_type>::type
    >::other allocator;

private:
    enum { effective_buffer_size =
          sizeof(char_type) * buffer_size > sizeof(char_type*)
        ? sizeof(char_type) * buffer_size
        : sizeof(char_type*)
        };
    enum { effective_buffer_alignment =
          buffer_alignment > boost::alignment_of<char_type*>::value
        ? buffer_alignment
        : boost::alignment_of<char_type*>::value
        };
    typedef boost::aligned_storage<effective_buffer_size, effective_buffer_alignment> aligned_storage;

    typedef size_t state_type;
    static state_type const shared_bit_mask = state_type(1) << (std::numeric_limits<state_type>::digits - 1);
    static state_type const allocated_bit_mask = shared_bit_mask >> 1;
    static state_type const size_bit_mask = allocated_bit_mask - 1;

public:
    enum { effective_buffer_size_chars = effective_buffer_size / sizeof(char_type) };

public:
    const_string_storage(char_type const* begin, size_t length, int /*reference_semantics*/)
    {
        if(length > this->max_size())
            throw std::length_error("const_string: the source string is way too long");

        state_ = length | shared_bit_mask;
        *this->as_shared() = begin;
    }

    const_string_storage(char_type const* begin, size_t length)
    {
        if(length > this->max_size())
            throw std::length_error("const_string: the source string is way too long");

        state_ = length
            | allocated_bit_mask
            | (length > effective_buffer_size_chars - 1 ? shared_bit_mask : 0)
            ;

        char_type* copy;

        if(this->is_shared())
        {
            size_t const character_bytes((length + 1) * sizeof(char_type));
            size_t const elements(
                  1
                + character_bytes / sizeof(typename allocator::value_type) 
                + (0 != character_bytes % sizeof(typename allocator::value_type))
                );

            void* const p(this->allocator::allocate(elements));
			new (p) boost::detail::atomic_count(1);
            copy = reinterpret_cast<char_type*>(reinterpret_cast<size_t>(p) + sizeof(typename allocator::value_type));
            *this->as_shared() = copy;
        }
        else
        {
            copy = this->as_buffer();
        }

        if(begin)
            TraitsT::copy(copy, begin, length);

        copy[length] = char_type();
    }

    const_string_storage(const_string_storage const& other) // throw()
        : allocator(other)
        , state_(other.state_)
    {
        if(this->is_shared())
        {
            *this->as_shared() = *other.as_shared();
            if(this->is_allocated())
                ++this->counter();
        }
        else
            TraitsT::copy(this->as_buffer(), other.as_buffer(), effective_buffer_size_chars);
    }

    const_string_storage const& operator=(const_string_storage const& other) // throw()
    {
        if(this != &other)
        {
            this->allocator::operator=(other);
            this->reset();
            new (this) const_string_storage(other);
        }
        return *this;
    }

    ~const_string_storage()
    {
        this->reset();
    }

public:
    const_string_storage& set_size(size_t length)
    {
        if(length > this->size())
            throw std::length_error("const_string: the source string is way too long");
        state_ = state_ & (allocated_bit_mask | shared_bit_mask) | length;
        return *this;
    }

public:
    size_t max_size() const
    {
        return size_bit_mask;
    }

    size_t size() const
    {
        return state_ & size_bit_mask;
    }

    char_type const* begin() const
    {
        return this->is_shared()
            ? *this->as_shared()
            : this->as_buffer()
            ;
    }

    char_type const* end() const
    {
        return this->begin() + this->size();
    }

private:
    void reset()
    {
        if((allocated_bit_mask | shared_bit_mask) == (state_ & (allocated_bit_mask | shared_bit_mask)))
        {
            if(0 == --this->counter())
			{
                size_t const character_bytes((this->size() + 1) * sizeof(char_type));
                size_t const elements(
                      1
                    + character_bytes / sizeof(typename allocator::value_type) 
                    + (0 != character_bytes % sizeof(typename allocator::value_type))
                    );

				boost::detail::atomic_count* const p(&this->counter());
				// g++ 3.2.3 needs it for the next line when atomic_count is long int
				using boost::detail::atomic_count; 
				p->~atomic_count();

                this->allocator::deallocate(reinterpret_cast<typename allocator::pointer>(p), elements);
			}
        }
        state_ = 0;
        *this->as_shared() = 0;
    }

    bool is_allocated() const
    {
        return 0 != (state_ & allocated_bit_mask);
    }

    bool is_shared() const
    {
        return 0 != (state_ & shared_bit_mask);
    }

    char_type* as_buffer() const
    {
        return static_cast<char_type*>(const_cast<aligned_storage&>(stg_).address()); 
    }

    char_type const** as_shared() const
    {
        return static_cast<char_type const**>(const_cast<aligned_storage&>(stg_).address()); 
    }

    boost::detail::atomic_count& counter()
    {
        return *reinterpret_cast<boost::detail::atomic_count*>(
            reinterpret_cast<typename allocator::pointer>(
                const_cast<char_type*>(*this->as_shared())
                ) - 1
            );
    }

private:
    aligned_storage stg_;
    state_type state_;
};

////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace boost

////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BOOST_CONST_STRING_DETAIL_STORAGE_HPP

////////////////////////////////////////////////////////////////////////////////////////////////


