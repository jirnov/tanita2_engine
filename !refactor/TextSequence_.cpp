#include <precompiled.h>
#include "TextSequence_.h"
#include "graphics_.h"

using namespace ingame;

// Sequence rendering
void TextSequence::doRender()
{
    Direct3D d3d;
    D3DXVECTOR4 np;
    D3DXVec2Transform(&np, &mPosition, &mTransformationMatrix);
    long px = long(np.x), py = long(np.y);
    RECT r = {px, py, px+1, py+1};
    d3d->text_drawer->DrawTextA(NULL, text.c_str(), -1, &r, DT_NOCLIP, color);
}
