/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ��������� ����������.
 */
#pragma once
#ifndef _FLAMENCO_CORE_CONFIG_H_
#define _FLAMENCO_CORE_CONFIG_H_

namespace flamenco
{

// �������� � ������������� ��� ������ �����.
// ������ �� ������ ������ ������������ � �������� �� ���������.
const u32 LATENCY_MSEC = 10;

// ������� �������������, ������������ ������ ���������� � �� �� ������.
const u32 FREQUENCY = 44100;

// ������ ������ ������������� ������ �������� � �������������.
const u32 DECODE_BUFFER_SIZE_IN_MSEC = 2000;


// ����������� ���������.

// ������ ������ ������������ ��� ������ ������ � ������� (������ 16 ������ ��� ��������� SSE).
const u32 CHANNEL_BUFFER_SIZE_IN_SAMPLES = (LATENCY_MSEC * FREQUENCY / 1000) / 4 * 4;

// ������ ������ ������������, ������������� ������� (� ������������� ��������), � �������.
const u32 MIXER_BUFFER_SIZE_IN_SAMPLES = CHANNEL_BUFFER_SIZE_IN_SAMPLES * 2;

} // namespace flamenco

#endif // _FLAMENCO_CORE_CONFIG_H_
