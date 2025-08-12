/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef __BASE_MPL_IF_H__
#define __BASE_MPL_IF_H__

namespace base {
namespace mpl
{

// Шаблон для выбора типа в зависимости от условия
// Пример:
//     mpl::if_<OBJ_COUNT > 1000, big_container, small_container>::type container;

template<bool C, class T, class F>
struct if_
{
    typedef F type;
};

template<class T, class F>
struct if_<true, T, F>
{
    typedef T type;
};

}} // base::mpl namespace

#endif // __BASE_MPL_IF_H__
