/*
 ������ "������2".
 �������� �������.
 */
#pragma once
#ifndef _TANITA2_RENDER_TEXTURE_TEXTURE_H_
#define _TANITA2_RENDER_TEXTURE_TEXTURE_H_

#include <boost/noncopyable.hpp>
#include <tanita2/render/texture/texturetypes.h>

namespace render
{

// �������� �������.
class TextureManager : boost::noncopyable
{
public:
    struct initializer
    {
        // ������������ ����� ������.
        u32 maxSystemMemory,
            maxVideoMemory;
    };

private:
    TextureManager( const initializer & initializer );
    friend class utility::manual_singleton<TextureManager>;
    
    // ���������� � ������ ���������� � �������������� ����� ����.
    void prepareToReset();
    void restoreAfterReset();
    // �������� �������������� ��������.
    void unloadUnusedResources();
    friend class Device;
    
    // ������ �������.
    typedef std::vector<Texture> TextureList;
    TextureList mTextures;
};

} // namespace render

#endif // _TANITA2_RENDER_TEXTURE_TEXTURE_H_
