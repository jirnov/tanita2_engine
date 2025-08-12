/**
 config
 Система хранения и загрузки настроек приложения.
 */
#pragma once
#ifndef _CONFIG_DETAIL_VALUE_H_
#define _CONFIG_DETAIL_VALUE_H_

namespace config { namespace detail
{

// Тип для значений, читаемых из файла настроек.
template<class value_type>
struct value
{
public:
    explicit value( const char * name, value_type defaultValue = value_type() )
        : mName(name), mValue(defaultValue), mIsLoaded(false)
    {
    }
    
    inline value_type operator ()() const
    {
        if (!mIsLoaded)
        {
            mValue = config::parser().get_value<value_type>(mName, mValue);
            mIsLoaded = true;
        }
        return mValue;
    }
    
private:
    const char * mName;
    mutable value_type mValue;
    mutable bool mIsLoaded;
};


// Специализация для const char * - используем безопасный строковый тип.
template<>
struct value<const char *> : value<const_string>
{
    explicit value( const char * name, const char * defaultValue = "" )
        : value<const_string>(name, defaultValue)
    {
    }
};
template<>
struct value<const wchar_t *> : value<const_wstring>
{
    explicit value( const char * name, const wchar_t * defaultValue = L"" )
        : value<const_wstring>(name, defaultValue)
    {
    }
};

}} // namespace config::detail

#endif // _CONFIG_DETAIL_VALUE_H_
