/*
 Движок "Танита2".
 Объявление парсера файла настроек.
 */
#pragma once
#ifndef _TANITA2_CONFIG_PARSER_H_
#define _TANITA2_CONFIG_PARSER_H_

namespace config
{

// Парсер настроек.
struct ConfigParser
{
    template<class T>
    T get_value( const char *, const T & defaultValue ) const
    {
        return defaultValue;
    }
};

// Возвращает парсер файла настроек.
inline const ConfigParser & parser()
{
    static ConfigParser parser;
    return parser;
}

} // namespace config

#endif // _TANITA2_CONFIG_PARSER_H_
