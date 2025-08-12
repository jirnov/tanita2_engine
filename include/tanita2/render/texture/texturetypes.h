/*
 Движок "Танита2".
 Определение типов текстур.
 */
#pragma once
#ifndef _TANITA2_RENDER_TEXTURE_TEXTURETYPES_H_
#define _TANITA2_RENDER_TEXTURE_TEXTURETYPES_H_

#include <tanita2/utility/refcountable.h>

namespace render
{

// Параметры текстуры.
struct TextureParameters
{
    // Используемая фильтрация.
    D3DTEXTUREFILTERTYPE minFilter, magFilter;
    // Формат текстуры.
    D3DFORMAT format;
    // Пул текстуры.
    D3DPOOL pool;
};

// Базовый класс для текстур.
class TextureResource : public utility::refcountable
{
public:
    virtual ~TextureResource() = 0 {}
    
    // Параметры текстуры.
    inline const TextureParameters & parameters() const
    {
        return mParameters;
    }
    
private:
    const TextureParameters mParameters;
    bool mIsLoaded;
    
    // Загрузка/выгрузка с проверкой флага mIsLoaded.
    void load();
    void unload();
    // Подготовка к сбросу и восстановление после него.
    virtual void prepareToReset() = 0;
    virtual void restoreAfterReset() = 0;
    
    // Загрузка текстуры и получение ее объекта D3D.
    LPDIRECT3DBASETEXTURE9 texture();
    friend class TextureManager;
    
protected:
    TextureResource( const TextureParameters & params )
        : mParameters(params), mIsLoaded(false)
        {}
    
    // Реализация функций load, unload и texture без проверок mIsLoaded.
    virtual void unloadImpl() = 0;
    virtual void loadImpl() = 0;
    virtual LPDIRECT3DBASETEXTURE9 textureImpl() = 0;
};
typedef utility::reference<TextureResource> Texture;

} // namespace render

#endif // _TANITA2_RENDER_TEXTURE_TEXTURETYPES_H_
