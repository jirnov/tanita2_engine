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
#include "ingame/textRenderer.h"

void exportTextRenderObject( void )
{
    using namespace bp;
    using namespace ingame;
    
    class_<TextRenderObject, bases<GameObject> >("TextRenderObject")
        .def("loadFont",     &TextRenderObject::loadFont)
        .def("setText",      &TextRenderObject::setText)
        .def("setTextWidth", &TextRenderObject::setTextWidth)
        .def("enableCenterAlign", &TextRenderObject::enableCenterAlign)
        .def("disableCenterAlign", &TextRenderObject::disableCenterAlign)
        
        .add_property("sequence", make_function(&TextRenderObject::getSequence,
                                      return_value_policy<copy_non_const_reference>()))
        ;
}

using namespace ingame;

// Загрузка секвенции с символами шрифта.
void TextRenderObject::loadFont( const DirectoryId & directoryPath,
                                         const bp::list & letterMetrics )
{
    ASSERT(NULL == mSequencePtr);
    
    U32 letterWidth[TextRenderSequence::GLYPHCOUNT];
    for (U32 i = 0; i < TextRenderSequence::GLYPHCOUNT; ++i)
        letterWidth[i] = bp::extract<U32>(letterMetrics[i]);
    
    mSequencePtr = new TextRenderSequence(directoryPath, letterWidth);
    mSequence = SequenceManagerSingleton()->registerSequence(mSequencePtr);
}

// Обновление состояния.
void TextRenderObject::update( F32 dt )
{
    begin_update();
    mSequence->addToRenderQueue(dt);
    update_children(dt);
    end_update();
}
