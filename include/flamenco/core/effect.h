/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ������ (���, ������� ���� � �����).
 */
#pragma once
#ifndef _FLAMENCO_CORE_EFFECT_H_
#define _FLAMENCO_CORE_EFFECT_H_

namespace flamenco
{

// ������ - ���, �������������� ������ ������� ����.
class effect : public pin
{
protected:
    // ������� ������ � ������������ � ���� ������� ���.
    effect( reference<pin> input );
    ~effect();
    
    // ������ �������� mInputPin.process() � ������������
    // ����������� �� ������. �� ����� � ��������� �� ��������������.
    virtual void process( f32 * left, f32 * right ) = 0;
    
    // ����� process() ��� �������� ����.
    inline void process_input_pin( f32 * left, f32 * right )
    {
        mInput->process(left, right);
    }
    
private:

    // ����������� � ������������� ��� ������������ ������� ���� � �������.
    void on_attach( bool connected )
    {
        mInput->on_attach(connected);
    }

    // ������� ���.
    reference<pin> mInput;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_EFFECT_H_
