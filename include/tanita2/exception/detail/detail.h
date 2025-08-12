/*
 Движок "Танита2".
 Реализация исключений и функций для работы с ними.
 */
#pragma once
#ifndef _TANITA2_EXCEPTION_DETAIL_DETAIL_H_
#define _TANITA2_EXCEPTION_DETAIL_DETAIL_H_

namespace exception_detail
{

// Базовый класс исключений движка.
class exception : public std::exception
{
protected:
    // Инициализация.
    exception( const char * file, int line, const char * function )
        : mFile(file), mCodeLine(line), mFunction(function)
    {
        assert(file);
        assert(function);
    }
    
    // Проверка на заполненность данных.
    inline bool empty() const
    {
        return NULL == mFile;
    }
    
    // Файл, строка и функция, где произошло исключение.
    inline const char * file() const
    {
        return mFile ? mFile : "None";
    }
    inline int line() const
    {
        return mCodeLine;
    }
    inline const char * function() const
    {
        return mFunction ? mFunction : "None";
    }
    
private:
    const char * mFile;
    int mCodeLine;
    const char * mFunction;
};

} // namespace exception_detail

#endif // _TANITA2_EXCEPTION_DETAIL_DETAIL_H_
