/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Источник данных из обычного файла.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_FILE_H_
#define _FLAMENCO_INPUT_FILE_H_

namespace flamenco
{

// Источник данных из обычного файла без буферизации чтения.
class file_source : public source
{
public:
    file_source( const char * path );
    ~file_source();

    // Копирует count элементов размером size в destination. 
    // В случае ошибки возращает ноль, иначе количество прочитанных элементов
    virtual u32 read( u8 * destination, u32 size, u32 count );

    // Пропускает определенное количество байтов.
    // offset может иметь отрицательное значение.
    virtual void skip( s32 offset );

    // Устанавливает курсор чтения offset от начала потока
    virtual void seek( u32 offset );

    // Возвращает текущее смещение курсора чтения
    virtual u32 tell() const;

    // Возвращает true, если поток подошел к концу
    virtual bool eof() const;
    
private:
    FILE * mFile;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_FILE_H_
