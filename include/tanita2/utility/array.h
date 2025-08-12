/*
 array.h
 Массив фиксированного максимального размера с изменяемым количеством элементов.
 */
#ifndef _TANITA2_UTILITY_ARRAY_H_
#define _TANITA2_UTILITY_ARRAY_H_

#include <cstddef>
#include <iterator>
#include <cassert>
#include <stdexcept>

namespace utility
{

// Код основан на boost::array из Boost 1.36.0
template<class T, std::size_t N>
class array
{
public:
    // Типы
    typedef T value_type;
    typedef T * iterator;
    typedef const T * const_iterator;
    typedef T & reference;
    typedef const T & const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    // Конструкторы и присваивание
    array() : mCount(0) {}
    
    // Работа с итераторами
    iterator begin()
    {
        return mElements;
    }
    const_iterator begin() const
    {
        return mElements;
    }
    iterator end()
    {
        return mElements + mCount;
    }
    const_iterator end() const
    {
        return mElements + mCount;
    }
    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    // Количество элементов
    size_type size() const
    {
        return mCount;
    }
    size_type max_size() const
    {
        return N;
    }
    size_type capacity() const
    {
        return N;
    }
    bool empty() const
    {
        return 0 == mCount;
    }

    // Доступ к элементам
    reference operator []( size_type i )
    {
        assert(i < mCount && "out of range");
        return mElements[i];
    }
    const_reference operator []( size_type i ) const
    {
        assert(i < mCount && "out of range");
        return mElements[i];
    }
    reference at( size_type i )
    {
        rangecheck(i);
        return mElements[i];
    }
    const_reference at( size_type i ) const
    {
        rangecheck(i);
        return mElements[i];
    }
    reference front()
    {
        assert(mCount > 0 && "array empty");
        return mElements[0]; 
    }
    const_reference front() const
    {
        assert(mCount > 0 && "array empty");
        return mElements[0];
    }
    reference back()
    {
        assert(mCount > 0 && "array empty");
        return mElements[mCount - 1];
    }
    const_reference back() const
    {
        assert(mCount > 0 && "array empty");
        return mElements[mCount - 1];
    }

    // Изменение
    void push_back( const T & x )
    {
        assert(mCount < N && "array size exceeded");
        mElements[mCount++] = x;
    }
    void pop_back()
    {
        assert(mCount > 0 && "array empty before pop");
        destroy(mElements + --mCount);
    }
    iterator insert( iterator position, const T & x )
    {
        assert(position >= begin() && position <= end() && "out of range");
        assert(mCount < N && "array size exceeded");
        std::copy(rbegin(), reverse_iterator(position) + 1, rbegin() - 1);
        *position = x;
        mCount++;
        return position;
    }
    iterator erase( iterator position )
    {
        assert(position >= begin() && position < end() && "out of range");
        assert(mCount > 0 && "array empty");
        std::copy(position + 1, end(), position);
        destroy(mElements + --mCount);
        return position;
    }
    void clear()
    {
        while (mCount > 0)
            destroy(mElements + --mCount);
    }
    
    
private:
    void rangecheck( size_type i ) const
    {
        if (i >= size())
            throw std::out_of_range("array<>: index out of range");
    }
    void destroy( T * ptr )
    {
        ptr->~T();
        new(ptr) T();
    }

    T mElements[N];
    std::size_t mCount;
};

// Запрещаем массивы нулевого размера.
template<class T>
class array<T, 0>
{
    // Запрещаем компиляцию.
    typedef typename T::array_of_zero_size_is_not_allowed error;
};

} // namespace utility

#endif // _TANITA2_UTILITY_ARRAY_H_
