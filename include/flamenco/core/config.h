/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Настройки библиотеки.
 */
#pragma once
#ifndef _FLAMENCO_CORE_CONFIG_H_
#define _FLAMENCO_CORE_CONFIG_H_

namespace flamenco
{

// Задержка в миллисекундах при выводе звука.
// Влияет на размер буфера микширования и нагрузку на процессор.
const u32 LATENCY_MSEC = 10;

// Частота дискретизации, используемая внутри библиотеки и на ее выходе.
const u32 FREQUENCY = 44100;

// Размер буфера декодирования сжатых форматов в миллисекундах.
const u32 DECODE_BUFFER_SIZE_IN_MSEC = 2000;


// Производные константы.

// Размер буфера микширования для одного канала в семплах (кратен 16 байтам для поддержки SSE).
const u32 CHANNEL_BUFFER_SIZE_IN_SAMPLES = (LATENCY_MSEC * FREQUENCY / 1000) / 4 * 4;

// Размер буфера микширования, передаваемого микшеру (с чередующимися каналами), в семплах.
const u32 MIXER_BUFFER_SIZE_IN_SAMPLES = CHANNEL_BUFFER_SIZE_IN_SAMPLES * 2;

} // namespace flamenco

#endif // _FLAMENCO_CORE_CONFIG_H_
