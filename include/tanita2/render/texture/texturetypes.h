/*
 ������ "������2".
 ����������� ����� �������.
 */
#pragma once
#ifndef _TANITA2_RENDER_TEXTURE_TEXTURETYPES_H_
#define _TANITA2_RENDER_TEXTURE_TEXTURETYPES_H_

#include <tanita2/utility/refcountable.h>

namespace render
{

// ��������� ��������.
struct TextureParameters
{
    // ������������ ����������.
    D3DTEXTUREFILTERTYPE minFilter, magFilter;
    // ������ ��������.
    D3DFORMAT format;
    // ��� ��������.
    D3DPOOL pool;
};

// ������� ����� ��� �������.
class TextureResource : public utility::refcountable
{
public:
    virtual ~TextureResource() = 0 {}
    
    // ��������� ��������.
    inline const TextureParameters & parameters() const
    {
        return mParameters;
    }
    
private:
    const TextureParameters mParameters;
    bool mIsLoaded;
    
    // ��������/�������� � ��������� ����� mIsLoaded.
    void load();
    void unload();
    // ���������� � ������ � �������������� ����� ����.
    virtual void prepareToReset() = 0;
    virtual void restoreAfterReset() = 0;
    
    // �������� �������� � ��������� �� ������� D3D.
    LPDIRECT3DBASETEXTURE9 texture();
    friend class TextureManager;
    
protected:
    TextureResource( const TextureParameters & params )
        : mParameters(params), mIsLoaded(false)
        {}
    
    // ���������� ������� load, unload � texture ��� �������� mIsLoaded.
    virtual void unloadImpl() = 0;
    virtual void loadImpl() = 0;
    virtual LPDIRECT3DBASETEXTURE9 textureImpl() = 0;
};
typedef utility::reference<TextureResource> Texture;

} // namespace render

#endif // _TANITA2_RENDER_TEXTURE_TEXTURETYPES_H_
