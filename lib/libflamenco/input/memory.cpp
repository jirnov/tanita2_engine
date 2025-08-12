/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� ��������� �� ������� ������.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// �����������
memory_source::memory_source( auto_array<u8> buffer, u32 size )
    : mBuffer(buffer), mOffset(0)
{
    assert(mBuffer.get());
    mSize = size;
}

// �������� count ��������� �������� size � destination. 
// � ������ ������ ��������� ����, ����� ���������� ����������� ���������
u32 memory_source::read( u8 * destination, u32 size, u32 count )
{
    assert(NULL != destination);

    u32 bytesRead = std::min(size * count, mSize - mOffset) / size * size;
    if (bytesRead)
    {
        memcpy(destination, mBuffer.get() + mOffset, bytesRead);
        skip(bytesRead);
    }
    return bytesRead / size;
}

// ������������� ������ ������ offset �� ������ ������
void memory_source::seek( u32 offset )
{
    mOffset = offset;
    if (mOffset > mSize)
        mOffset = mSize;
}

// ���������� ������������ ���������� ������.
// offset ����� ����� ������������� ��������.
void memory_source::skip( s32 offset )
{
    mOffset += offset;
    if (mOffset > mSize)
        mOffset = mSize;
}

// ���������� ������� �������� ������� ������
u32 memory_source::tell() const
{
    return mOffset;
}

// ���������� true, ���� ����� ������� � �����
bool memory_source::eof() const
{
    return mOffset == mSize;
}
