/*
 ������ "������2".
 ���������� ����������� ���������.
 */
#include <precompiled.h>
#include <tanita2/render/texture/texture.h>

using namespace render;

// �����������.
TextureManager::TextureManager( const TextureManager::initializer & )
{

}

// ���������� � ������ ����������.
void TextureManager::prepareToReset()
{
    for (TextureList::iterator i = mTextures.begin(); mTextures.end() != i; ++i)
        (*i)->restoreAfterReset();
}

// �������������� ����� ������.
void TextureManager::restoreAfterReset()
{
    for (TextureList::iterator i = mTextures.begin(); mTextures.end() != i; ++i)
        (*i)->restoreAfterReset();
}

// �������� �������������� ��������.
void TextureManager::unloadUnusedResources()
{
}
