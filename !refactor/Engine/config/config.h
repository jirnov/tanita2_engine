/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef __CONFIG_CONFIG_H__
#define __CONFIG_CONFIG_H__

#include "base/types.h"

//! Пространство имен для конфигурационных значений
namespace config
{

//! Показатель отладочного режима
const bool DEBUG =
#if defined(_DEBUG) || defined(_RELEASE_WITH_DEBUG)
    true;
#else
    false;
#endif

//! Настройки графики
namespace gfx
{
    //! Логическая ширина экрана.
    const U32 SCREEN_WIDTH = 1024;
    //! Логическая высота экрана.
    const U32 SCREEN_HEIGHT = 768;
}

//! Настройки звуковой системы
namespace sound
{
    //! Частота дискретизации первичного буфера.
    const U16 PRIMARY_SAMPLE_RATE = 44100;
    //! Количество бит на отсчет для первичного буфера.
    const U16 PRIMARY_BITS_PER_SAMPLE = 16;
    //! Максимальное количество групп звуков.
    const U32 MAX_GROUP_COUNT = 5;
    
    //! Длина поточного буфера в миллисекундах.
    const U32 STREAMING_BUFFER_LENGTH_MSEC = 10000;
    
    //! Допустимое расстояние до конца звука для его остановки (в байтах).
    const U32 END_SPACING = (U32)4000;
    
    //! Частота удаления ненужных звуков, в миллисекундах.
    const U32 CLEANUP_PERIOD = 2500;
}

//! Настройки межпоточной синхронизации
namespace sync
{
    //! Максимальное время ожидания блокировки, в миллисекундах.
    const U32 MAX_WAIT_TIME = 10000;
}

// Настройки ресурсной системы.
namespace res
{
    //! Базовое имя для ресурс-пака (имена образуются добавлением номера)
    static const char * RESOURCE_FILE_BASENAME = "Data\\Resource";
    //! Расширение файла с заголовком ресурс-пака
    static const char * RESOURCE_TOC_EXTENSION = ".toc";
    //! Расширение файлов ресурс-пака
    static const char * RESOURCE_FILE_EXTENSION = ".pak";
    //! Значение хешей идентификатора ресурса по умолчанию
    const U32 RESOURCEID_INVALID_HASH = (U32)0xffffffff;
}

} // namespace config

#endif // __CONFIG_CONFIG_H__
