/*
 Движок "Танита2".
 Реализация текстурного менеджера.
 */
#include <precompiled.h>
#include <tanita2/render/texture/texture.h>

using namespace render;

// Конструктор.
TextureManager::TextureManager( const TextureManager::initializer & )
{

}

// Подготовка к сбросу устройства.
void TextureManager::prepareToReset()
{
    for (TextureList::iterator i = mTextures.begin(); mTextures.end() != i; ++i)
        (*i)->restoreAfterReset();
}

// Восстановление после сброса.
void TextureManager::restoreAfterReset()
{
    for (TextureList::iterator i = mTextures.begin(); mTextures.end() != i; ++i)
        (*i)->restoreAfterReset();
}

// Выгрузка неиспользуемых ресурсов.
void TextureManager::unloadUnusedResources()
{
}
