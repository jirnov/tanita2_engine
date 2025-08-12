/* Trickster Games Technology
 * (c) Trickster Games, 2008
 */
#include <precompiled.h>
#include "Graphics_.h"
#include "EffectsManager_.h"
#include "graphics/sequenceManager/sequenceManager.h"

#include "Python_.h"
#include "Application_.h"
#include "Sequence_.h"
#include "GameObject_.h"
#include "ingame/textSequence.h"

// Конструктор.
ingame::TextRenderSequence::TextRenderSequence( const DirectoryId & textureDirectory, U32 glyphWidth[GLYPHCOUNT] )
    : mWidth(0), mEnableCenterAlign(false)
{
    // Загрузка текстуры нулевого символа.
    TextureManager tm;
    TRY(mGlyphTexture[0] = tm->load(textureDirectory.toResourceId(0), true));
    DDSURFACEDESC2 desc = mGlyphTexture[0]->get_description();
    mBoundingBox.x = static_cast<float>(desc.dwReserved & 0xFFFF);
    mBoundingBox.y = static_cast<float>(desc.dwReserved >> 16);
    
    // Создание вершинного буфера.
    mVertexBuffer = SequenceManagerSingleton()->create(desc.dwWidth, desc.dwHeight);
    
    
    // Загрузка остальных символов.
    for (int i = 1; i < GLYPHCOUNT; ++i)
        TRY(mGlyphTexture[i] = tm->load(textureDirectory.toResourceId(i), true));
    
    mText[0] = 0;
    memcpy(mGlyphWidth, glyphWidth, sizeof(U32) * GLYPHCOUNT);
}

// Отрисовка секвенции.
void ingame::TextRenderSequence::doRender()
{
    F32 alignWidth = 0;

    if (mEnableCenterAlign)
    {
        F32 totalWidth = 0;
        
        const char * textPtr = mText;
        while (*textPtr != 0)
            totalWidth += mGlyphWidth[(unsigned char)*textPtr++];
        
        if ((totalWidth > mWidth * 0.75) && (totalWidth < mWidth))
            alignWidth = (mWidth - totalWidth) / (F32)strlen(mText);
    }
    
    D3DXMATRIX m = mTransformationMatrix;
    const char * textPtr = mText;

    while (*textPtr != 0)
    {
        unsigned char c = (unsigned char)*textPtr++;
        Direct3D d3d;
        d3d->device->SetTransform(D3DTS_WORLD, &m);
        mGlyphTexture[c]->bind(0);
        mVertexBuffer->draw();
        m[12] += mGlyphWidth[c] + alignWidth;
    }
}
