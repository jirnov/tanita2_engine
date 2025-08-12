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

// Секвенция для отрисовки текста.
class TextRenderSequence : public graphics::SequenceBase
{
public:
    enum {
        // Максимальное количество символов в строке.
        MAXCHARCOUNT = 256,
        
        // Количество символов алфавита.
        GLYPHCOUNT = 256,
    };
    
    
    // Конструктор.
    inline TextRenderSequence() {}
    
    // Загрузка шрифта из набора текстур и размеров букв (метрики).
    TextRenderSequence( const DirectoryId & textureDirectory, U32 glyphWidth[GLYPHCOUNT] );
    
    // Установка текста для отрисовки.
    inline void setText( const char * text ) { strncpy(mText, text, MAXCHARCOUNT); }

    // Установка ширины текста для выравнивания.
    inline void setTextWidth( F32 width ) { mWidth = width; }

    // Включение выравнивания.
    inline void enableCenterAlign() { mEnableCenterAlign = true; }

    // Выключение выравнивания.
    inline void disableCenterAlign() { mEnableCenterAlign = false; }

    
private:
    // Отрисовка
    void doRender();
    
    // Текстуры символов алфавита.
    TextureRef mGlyphTexture[GLYPHCOUNT];
    // Ширина каждой буквы.
    U32 mGlyphWidth[GLYPHCOUNT];
    // Общая ширина строки для выравнивания по центру
    F32 mWidth;
    // Выравнивание по центру
    bool mEnableCenterAlign;
    
    // Вершинный буфер.
    VertexBufferRef mVertexBuffer;
    
    // Текст.
    char mText[MAXCHARCOUNT];
};

}

#endif // _INGAME_SEQUENCE_TEXTSEQUENCE_H_
