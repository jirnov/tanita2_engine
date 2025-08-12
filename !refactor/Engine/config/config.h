/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef __CONFIG_CONFIG_H__
#define __CONFIG_CONFIG_H__

#include "base/types.h"

//! ������������ ���� ��� ���������������� ��������
namespace config
{

//! ���������� ����������� ������
const bool DEBUG =
#if defined(_DEBUG) || defined(_RELEASE_WITH_DEBUG)
    true;
#else
    false;
#endif

//! ��������� �������
namespace gfx
{
    //! ���������� ������ ������.
    const U32 SCREEN_WIDTH = 1024;
    //! ���������� ������ ������.
    const U32 SCREEN_HEIGHT = 768;
}

//! ��������� �������� �������
namespace sound
{
    //! ������� ������������� ���������� ������.
    const U16 PRIMARY_SAMPLE_RATE = 44100;
    //! ���������� ��� �� ������ ��� ���������� ������.
    const U16 PRIMARY_BITS_PER_SAMPLE = 16;
    //! ������������ ���������� ����� ������.
    const U32 MAX_GROUP_COUNT = 5;
    
    //! ����� ��������� ������ � �������������.
    const U32 STREAMING_BUFFER_LENGTH_MSEC = 10000;
    
    //! ���������� ���������� �� ����� ����� ��� ��� ��������� (� ������).
    const U32 END_SPACING = (U32)4000;
    
    //! ������� �������� �������� ������, � �������������.
    const U32 CLEANUP_PERIOD = 2500;
}

//! ��������� ����������� �������������
namespace sync
{
    //! ������������ ����� �������� ����������, � �������������.
    const U32 MAX_WAIT_TIME = 10000;
}

// ��������� ��������� �������.
namespace res
{
    //! ������� ��� ��� ������-���� (����� ���������� ����������� ������)
    static const char * RESOURCE_FILE_BASENAME = "Data\\Resource";
    //! ���������� ����� � ���������� ������-����
    static const char * RESOURCE_TOC_EXTENSION = ".toc";
    //! ���������� ������ ������-����
    static const char * RESOURCE_FILE_EXTENSION = ".pak";
    //! �������� ����� �������������� ������� �� ���������
    const U32 RESOURCEID_INVALID_HASH = (U32)0xffffffff;
}

} // namespace config

#endif // __CONFIG_CONFIG_H__
