/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация пина.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// Конструктор.
pin::pin()
    : mIsConnected(false)
{
}

// Деструктор.
pin::~pin()
{
    assert(!mIsConnected);
}
