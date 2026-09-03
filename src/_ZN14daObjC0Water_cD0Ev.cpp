//cpp
// @symbol _ZN14daObjC0Water_cD0Ev
/* Companion to _ZN14daObjC0Water_cD1Ev.cpp: a delete expression forces the
 * compiler-spelled DELETING destructor. The body is entirely compiler-written
 * from the inline destructor in include/daObjC0Water_c.h -- destroy through
 * this class and its bases, then return the object to its heap through an
 * inlined operator delete, which is why nothing here mentions a heap.
 */
#include "daObjC0Water_c.h"

void daObjC0Water_c_EmitDeletingDestructor(daObjC0Water_c *water) { delete water; }
