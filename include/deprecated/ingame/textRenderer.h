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

// ������ ��� ��������� ������ ������.
class TextRenderObject : public GameObject
{
public:
    // �����������.
    inline TextRenderObject()
        : mSequencePtr(NULL)
        {}
        
    // ���������� ���������.
    void update( F32 dt );
    
    // �������� ��������� � ��������� ������.
    void loadFont( const DirectoryId & path, const bp::list & letterMetrics );
    
    // ��������� ��������������� ������.
    inline void setText( const bp::str & text )
        { mText = bp::extract<const char *>(text);
          mSequencePtr->setText(mText.c_str()); }

    // ��������� ������ ��� ������������ ������.
    inline void setTextWidth( F32 Width )
        { mSequencePtr->setTextWidth(Width); }

    // ��������� ������������ ������
    inline void enableCenterAlign()
        { mSequencePtr->enableCenterAlign(); }

    // ���������� ������������
    inline void disableCenterAlign()
        { mSequencePtr->disableCenterAlign(); }
    
    // ��������� ���������.
    inline graphics::SequenceRef & getSequence() { return mSequence; }
    
    
private:
    // ������ � �������.
    std::string mText;
    // ��������� �� ���������.
    graphics::SequenceRef mSequence;
    TextRenderSequence * mSequencePtr;

private:
    friend void ::exportTextRenderObject( void );
};

}

#endif // _INGAME_TEXTRENDERER_H_
