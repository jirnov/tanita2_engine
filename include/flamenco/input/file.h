/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 �������� ������ �� �������� �����.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_FILE_H_
#define _FLAMENCO_INPUT_FILE_H_

namespace flamenco
{

// �������� ������ �� �������� ����� ��� ����������� ������.
class file_source : public source
{
public:
    file_source( const char * path );
    ~file_source();

    // �������� count ��������� �������� size � destination. 
    // � ������ ������ ��������� ����, ����� ���������� ����������� ���������
    virtual u32 read( u8 * destination, u32 size, u32 count );

    // ���������� ������������ ���������� ������.
    // offset ����� ����� ������������� ��������.
    virtual void skip( s32 offset );

    // ������������� ������ ������ offset �� ������ ������
    virtual void seek( u32 offset );

    // ���������� ������� �������� ������� ������
    virtual u32 tell() const;

    // ���������� true, ���� ����� ������� � �����
    virtual bool eof() const;
    
private:
    FILE * mFile;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_FILE_H_
