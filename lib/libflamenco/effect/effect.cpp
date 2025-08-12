/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация эффекта.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// Создает эффект и присоединяет к нему входной пин.
effect::effect( reference<pin> input )
{
    assert(!input->connected());
    mInput = input;
    mInput->set_connected(true);
}

// Деструктор.
effect::~effect()
{
    assert(mInput->connected());
    mInput->set_connected(false);
}
