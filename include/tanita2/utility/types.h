/*
 Движок "Танита2".
 Описания примитивных типов.
 */
#pragma once
#ifndef _TANITA2_UTILITY_TYPES_H_
#define _TANITA2_UTILITY_TYPES_H_

// Целые типы фиксированного размера.
typedef unsigned char   u8;
typedef signed char     s8;
typedef unsigned short  u16;
typedef signed short    s16;
typedef unsigned long   u32;
typedef signed long     s32;

// Типы с плавающей точкой.
typedef float           f32;
typedef double          f64;

// Тип для неизменяемой строки.
typedef boost::const_string<char> const_string;
typedef boost::const_string<wchar_t> const_wstring;

#endif // _TANITA2_UTILITY_TYPES_H_
