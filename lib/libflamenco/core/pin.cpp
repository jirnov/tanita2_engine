/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� ����.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// �����������.
pin::pin()
    : mIsConnected(false)
{
}

// ����������.
pin::~pin()
{
    assert(!mIsConnected);
}
