//cpp
// @symbol _ZN13PeachPaintingD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "PeachPainting.h"

void PeachPainting_EmitDeletingDestructor(PeachPainting *painting)
{
    delete painting;
}
