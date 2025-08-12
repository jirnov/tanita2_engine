/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef _RESOURCE_HEADERS_H_
#define _RESOURCE_HEADERS_H_


//! ��������� DDS-�����
struct ResourceHeaderDDS
{
    //! ��� ��������� (������ RESOURCE_TYPE_DDS)
    ResourceType type;
    
    U32 width,             //!< ������ ��������, ����������� �� ������� ������.
        height,            //!< ������ ��������, ����������� �� ������� ������.
        boundingBoxWidth,  //!< ������ �������� ��������.
        boundingBoxHeight, //!< ������ �������� ��������.
        isDxtCompressed;   //!< ������������ �� ���������� ������.
};

//! ��������� OGG �����
struct ResourceHeaderOGG
{
    //! ��� ��������� (������ RESOURCE_TYPE_OGG)
    ResourceType type;
    
    //! ������������ �� ����������
    bool hasVideoStream;
    //! ����������� � �����������
    th_comment videoComment;
    //! ���������� � �����������
    th_info videoInfo;
    th_setup_info   *ts;
    th_dec_ctx      *td;
    
    //! ������������ �� ����������
    bool hasAudioStream;
    //! ����������� � �����������
    vorbis_comment audioComment;
    //! ���������� � �����������
    vorbis_info audioInfo;
};

#endif // _RESOURCE_HEADERS_H_
