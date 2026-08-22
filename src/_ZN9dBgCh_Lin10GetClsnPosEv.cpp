//cpp
// @symbol _ZN9dBgCh_Lin10GetClsnPosEv
/* recovered: named members */
/* Stays a mangled free definition: a real C++ method returning Vector3 by
   value cannot reproduce this shape -- mwcc builds the temporary on the
   stack instead of writing straight through the hidden return pointer, same
   "wall 6az" class as a Fix12<int> by-value parameter. The declaration in
   dBgCh_Lin.h is the real one and callers may use it.

   Returns lineEnd by value. Named for its role AFTER a hit, when
   SetObjAndLine's collision-point write has overwritten the field; see
   dBgCh_Lin.h for why the storage keeps the name `lineEnd`. */
#include "dBgCh_Lin.h"

extern "C" void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3 *res, dBgCh_Lin *self)
{
    res->x = self->lineEnd.x;
    res->y = self->lineEnd.y;
    res->z = self->lineEnd.z;
}
