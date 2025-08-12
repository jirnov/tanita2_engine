/* Trickster Games Technology
 * (c) Trickster Games, 2008
 */

#ifndef _INGAME_SEQUENCE_TEXTSEQUENCE_H_
#define _INGAME_SEQUENCE_TEXTSEQUENCE_H_

#include "graphics/sequenceManager/sequenceManager.h"
#include "graphics/graphics.h"
#include "GameObject_.h"

namespace ingame
{

// ��������� ��� ��������� ������.
class TextRenderSequence : public graphics::SequenceBase
{
public:
    enum {
        // ������������ ���������� �������� � ������.
        MAXCHARCOUNT = 256,
        
        // ���������� �������� ��������.
        GLYPHCOUNT = 256,
    };
    
    
    // �����������.
    inline TextRenderSequence() {}
    
    // �������� ������ �� ������ ������� � �������� ���� (�������).
    TextRenderSequence( const DirectoryId & textureDirectory, U32 glyphWidth[GLYPHCOUNT] );
    
    // ��������� ������ ��� ���������.
    inline void setText( const char * text ) { strncpy(mText, text, MAXCHARCOUNT); }

    // ��������� ������ ������ ��� ������������.
    inline void setTextWidth( F32 width ) { mWidth = width; }

    // ��������� ������������.
    inline void enableCenterAlign() { mEnableCenterAlign = true; }

    // ���������� ������������.
    inline void disableCenterAlign() { mEnableCenterAlign = false; }

    
private:
    // ���������
    void doRender();
    
    // �������� �������� ��������.
    TextureRef mGlyphTexture[GLYPHCOUNT];
    // ������ ������ �����.
    U32 mGlyphWidth[GLYPHCOUNT];
    // ����� ������ ������ ��� ������������ �� ������
    F32 mWidth;
    // ������������ �� ������
    bool mEnableCenterAlign;
    
    // ��������� �����.
    VertexBufferRef mVertexBuffer;
    
    // �����.
    char mText[MAXCHARCOUNT];
};

}

#endif // _INGAME_SEQUENCE_TEXTSEQUENCE_H_
