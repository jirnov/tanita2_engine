/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 �������� ������ �� ������� ������.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_MEMORY_H_
#define _FLAMENCO_INPUT_MEMORY_H_

namespace flamenco
{

// �������� ������ �� ������� ������.
class memory_source : public source
{
public:
    memory_source( auto_array<u8> buffer, u32 size );
    
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
    auto_array<u8> mBuffer;
    u32 mOffset;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_FILE_H_
