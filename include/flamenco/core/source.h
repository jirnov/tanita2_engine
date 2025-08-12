/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 �������� ������ ��� ��������.
 */
#pragma once
#ifndef _FLAMENCO_CORE_SOURCE_H_
#define _FLAMENCO_CORE_SOURCE_H_

namespace flamenco
{

// ������� ����� ��� ���������� ������ ��� ��������.
// � ������ ������ ������� ����������-���������� std::exception.
class source : noncopyable
{
public:
    virtual ~source() = 0 {}
    
    // �������� count ��������� �������� size � destination. 
    // � ������ ������ ��������� ����, ����� ���������� ����������� ���������
    virtual u32 read( u8 * destination, u32 size, u32 count ) = 0;

    // ���������� ������������ ���������� ������.
    // offset ����� ����� ������������� ��������.
    virtual void skip( s32 offset ) = 0;

    // ������������� ������ ������ offset �� ������ ������
    virtual void seek( u32 offset ) = 0;

    // ���������� ������� �������� ������� ������
    virtual u32 tell() const = 0;

    // ���������� true, ���� ����� ������� � �����
    virtual bool eof() const = 0;

    // ���������� ������ ������ � ������, ���� ���� ������ ���������� ������
    u32 size() const { return mSize; }

protected:
    u32 mSize;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_SOURCE_H_
