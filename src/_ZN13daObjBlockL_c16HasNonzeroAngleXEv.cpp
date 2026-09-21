//cpp
// @symbol _ZN13daObjBlockL_c16HasNonzeroAngleXEv
#include "daObjBlockL_c.h"

/* Both callers are daObjBlockL_c methods in this TU. The field at 0x8c is the
   inherited dActor_c::mAngleX; no external state participates. */
bool daObjBlockL_c::HasNonzeroAngleX()
{
    return mAngleX != 0;
}
