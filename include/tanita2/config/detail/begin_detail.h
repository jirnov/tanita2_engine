/**
 config
 Система хранения и загрузки настроек приложения.
 */
// Этот файл включается внутри другого .h файла и не нуждается в guards.

#ifndef _TANITA2_CONFIG_CONFIG_H_
#error begin_detail.h included prom improper place.
#endif // _TANITA2_CONFIG_CONFIG_H_

#include <tanita2/config/parser.h>
#include <tanita2/config/detail/value.h>

// Не забывайте делать #undef в end_detail.h

#define CONFIG_VALUE(name, type, default)                          \
    static struct name : config::detail::value<type>               \
    {                                                              \
        name() : config::detail::value<type>(#name, default) {}    \
    } name

// Объявление статической настройки.
#define CONFIG_STATIC_VALUE(name, value_type)                      \
    struct name##_type                                             \
    {                                                              \
        inline value_type operator ()() const { return smValue; }  \
    private:                                                       \
        static value_type smValue;                                 \
    };                                                             \
    static name##_type name;

// Значение типа size_t. Может использоваться на этапе компиляции,
// поэтому должно быть статическим константным членом.
#define CONFIG_STATIC_SIZE_T_VALUE(name, value_)                   \
    static struct name                                             \
    {                                                              \
        inline size_t operator ()() const { return value; }        \
        static const size_t value = value_;                        \
    } name
