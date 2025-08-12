/* Trickster Games Technology
 * (c) Trickster Games, 2008
 */

#ifndef _INGAME_TEXTRENDERER_H_
#define _INGAME_TEXTRENDERER_H_

#include "gameObject_.h"
#include "ingame/textSequence.h"

void exportTextRenderObject( void );

namespace ingame
{

// Объект для отрисовки строки текста.
class TextRenderObject : public GameObject
{
public:
    // Конструктор.
    inline TextRenderObject()
        : mSequencePtr(NULL)
        {}
        
    // Обновление состояния.
    void update( F32 dt );
    
    // Загрузка секвенции с символами шрифта.
    void loadFont( const DirectoryId & path, const bp::list & letterMetrics );
    
    // Изменение отрисовываемого текста.
    inline void setText( const bp::str & text )
        { mText = bp::extract<const char *>(text);
          mSequencePtr->setText(mText.c_str()); }

    // Изменение ширины для выравнивания текста.
    inline void setTextWidth( F32 Width )
        { mSequencePtr->setTextWidth(Width); }

    // Включение выравнивания текста
    inline void enableCenterAlign()
        { mSequencePtr->enableCenterAlign(); }

    // Отключение выравнивания
    inline void disableCenterAlign()
        { mSequencePtr->disableCenterAlign(); }
    
    // Получение секвенции.
    inline graphics::SequenceRef & getSequence() { return mSequence; }
    
    
private:
    // Строка с текстом.
    std::string mText;
    // Указатель на секвенцию.
    graphics::SequenceRef mSequence;
    TextRenderSequence * mSequencePtr;

private:
    friend void ::exportTextRenderObject( void );
};

}

#endif // _INGAME_TEXTRENDERER_H_
