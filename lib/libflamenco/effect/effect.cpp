/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� �������.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// ������� ������ � ������������ � ���� ������� ���.
effect::effect( reference<pin> input )
{
    assert(!input->connected());
    mInput = input;
    mInput->set_connected(true);
}

// ����������.
effect::~effect()
{
    assert(mInput->connected());
    mInput->set_connected(false);
}
