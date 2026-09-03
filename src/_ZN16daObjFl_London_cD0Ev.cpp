//cpp
// @symbol _ZN16daObjFl_London_cD0Ev
/* Companion to _ZN16daObjFl_London_cD1Ev.cpp: a delete expression forces the
 * compiler-spelled DELETING destructor. The body is entirely compiler-written
 * from the inline destructor in include/daObjFl_London_c.h -- destroy through
 * this class and its bases, then return the object to its heap through an
 * inlined operator delete, which is why nothing here mentions a heap.
 */
#include "daObjFl_London_c.h"

void daObjFl_London_c_EmitDeletingDestructor(daObjFl_London_c *london) { delete london; }
