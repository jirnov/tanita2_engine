/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ����������� ��������� �����.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_PROCEDURAL_H_
#define _FLAMENCO_INPUT_PROCEDURAL_H_

namespace flamenco
{

// �������� �������������� �����.
class sine : public pin
{
public:
    // �������� ������ ��������� �������������� ����� � �������� frequency ��.
    static reference<sine> create( u32 frequency );
    
    
    // ������� ���������.
    atomic<u32> frequency;
    
private:
    sine( u32 frequency );
    
    // ��������� ������ ������� ��������� �������.
    void process( f32 * left, f32 * right );
    
    // ��������� ����� ���������.
    u32 mTimeShift;
    
    // frequency ������� ��������� ���������� ��� get/set, �.�. atomic_u32 ���
    // �������� ���� get � set, ��� ��� �� ����� �� �����.
};

// �������� ������ ����.
class noise : public pin
{
public:
    // �������� ��������� ������ ����.
    static reference<noise> create();
    
private:
    noise() {}
    
    // ��������� ������ ������� ���������� ����������.
    void process( f32 * left, f32 * right );
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_PROCEDURAL_H_
