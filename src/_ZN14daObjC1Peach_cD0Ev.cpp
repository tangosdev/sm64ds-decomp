//cpp
// @symbol _ZN14daObjC1Peach_cD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "daObjC1Peach_c.h"

void PeachPainting_EmitDeletingDestructor(daObjC1Peach_c *painting)
{
    delete painting;
}
