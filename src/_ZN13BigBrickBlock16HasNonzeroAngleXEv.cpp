//cpp
// @symbol _ZN13BigBrickBlock16HasNonzeroAngleXEv
#include "BigBrickBlock.h"

/* Both callers are BigBrickBlock methods in this TU. The field at 0x8c is the
   inherited dActor_c::mAngleX; no external state participates. */
bool BigBrickBlock::HasNonzeroAngleX()
{
    return mAngleX != 0;
}
