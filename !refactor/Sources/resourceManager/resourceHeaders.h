/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef _RESOURCE_HEADERS_H_
#define _RESOURCE_HEADERS_H_


//! Заголовок DDS-файла
struct ResourceHeaderDDS
{
    //! Тип заголовка (всегда RESOURCE_TYPE_DDS)
    ResourceType type;
    
    U32 width,             //!< Ширина текстуры, выравненная по степени двойки.
        height,            //!< Высота текстуры, выравненная по степени двойки.
        boundingBoxWidth,  //!< Ширина исходной текстуры.
        boundingBoxHeight, //!< Высота исходной текстуры.
        isDxtCompressed;   //!< Используется ли текстурное сжатие.
};

//! Заголовок OGG файла
struct ResourceHeaderOGG
{
    //! Тип заголовка (всегда RESOURCE_TYPE_OGG)
    ResourceType type;
    
    //! Присутствует ли видеопоток
    bool hasVideoStream;
    //! Комментарий к видеопотоку
    th_comment videoComment;
    //! Информация о видеопотоке
    th_info videoInfo;
    th_setup_info   *ts;
    th_dec_ctx      *td;
    
    //! Присутствует ли аудиопоток
    bool hasAudioStream;
    //! Комментарий к аудиопотоку
    vorbis_comment audioComment;
    //! Информация о аудиопотоке
    vorbis_info audioInfo;
};

#endif // _RESOURCE_HEADERS_H_
