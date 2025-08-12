/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� ��������� ������ �� �����.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// �������� �����.
file_source::file_source( const char * path )
    : mFile(NULL)
{
    if (NULL == (mFile = fopen(path, "rb")))
        throw std::runtime_error("Unable to open file.");

    fseek(mFile, 0, SEEK_END);
    mSize = ftell(mFile);
    fseek(mFile, 0, SEEK_SET);
}

// ����������.
file_source::~file_source()
{
    if (NULL != mFile)
        fclose(mFile);
}

// �������� count ��������� �������� size � destination. 
// � ������ ������ ��������� ����, ����� ���������� ����������� ���������
u32 file_source::read( u8 * destination, u32 size, u32 count )
{
    assert(NULL != mFile);
    assert(NULL != destination);
    return static_cast<u32>(fread(destination, size, count, mFile));
}

// ������������� ������ ������ offset �� ������ ������
void file_source::seek( u32 offset )
{
    assert(NULL != mFile);
    fseek(mFile, offset, SEEK_SET);
}

// ���������� ������������ ���������� ������.
// offset ����� ����� ������������� ��������.
void file_source::skip( s32 offset )
{
    assert(NULL != mFile);
    fseek(mFile, offset, SEEK_CUR);
}

// ���������� ������� �������� ������� ������
u32 file_source::tell() const
{
    assert(NULL != mFile);
    return ftell(mFile);
}

// ���������� true, ���� ����� ������� � �����
bool file_source::eof() const
{
    assert(NULL != mFile);
    return feof(mFile) != 0;
}
