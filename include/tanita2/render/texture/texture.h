/*
 Движок "Танита2".
 Менеджер текстур.
 */
#pragma once
#ifndef _TANITA2_RENDER_TEXTURE_TEXTURE_H_
#define _TANITA2_RENDER_TEXTURE_TEXTURE_H_

#include <boost/noncopyable.hpp>
#include <tanita2/render/texture/texturetypes.h>

namespace render
{

// Менеджер текстур.
class TextureManager : boost::noncopyable
{
public:
    struct initializer
    {
        // Используемый объем памяти.
        u32 maxSystemMemory,
            maxVideoMemory;
    };

private:
    TextureManager( const initializer & initializer );
    friend class utility::manual_singleton<TextureManager>;
    
    // Подготовка к сбросу устройства и восстановление после него.
    void prepareToReset();
    void restoreAfterReset();
    // Выгрузка неиспользуемых ресурсов.
    void unloadUnusedResources();
    friend class Device;
    
    // Список текстур.
    typedef std::vector<Texture> TextureList;
    TextureList mTextures;
};

} // namespace render

#endif // _TANITA2_RENDER_TEXTURE_TEXTURE_H_
