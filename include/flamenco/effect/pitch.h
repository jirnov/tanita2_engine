/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ������ ��� ��������� ��������� � ���������������.
 */
#pragma once
#ifndef _FLAMENCO_EFFECT_PITCH_H_
#define _FLAMENCO_EFFECT_PITCH_H_

namespace flamenco
{

// ������ ��� ��������� �������� ������������ �����.
class pitch : public effect
{
public:
    // �������� ������ �������. �������� �������� �� 0.25 �� 4.0,
    // 0.25 - ���������� �������� � ������ ����
    // 4.0  - ���������� �������� � ������ ����
    static reference<pitch> create( reference<pin> input, f32 pitch_value=1.0f );
    
    // �������� � ��������� �� 0.25 �� 4.0.
    atomic<f32> pitch_value;
    
private:
    pitch( reference<pin> input, f32 pitch_value );
    
    // ��������� ������ ������� ��������� �������.
    void process( f32 * left, f32 * right );

    // �������� ������ �������
    u32 mBufferOffset;

    // ������ ������
    u32 mBufferSize;

    template <class T>
    struct aligned_deleter
    {
        void operator()(f32 * ptr)
        {
            _aligned_free(ptr);
        }
    };
    
    // ���������� ������ ��� �������
    auto_array<f32, aligned_deleter> mBufferL, mBufferR;

    // ��������� �������� ��� �������� ������ �� ������� �������.
    static const f32 MAGIC;
};

} // namespace flamenco

#endif // _FLAMENCO_EFFECT_PITCH_H_
