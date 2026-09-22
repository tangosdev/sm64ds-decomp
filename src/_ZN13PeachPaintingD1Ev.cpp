//cpp
// @symbol _ZN13PeachPaintingD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
#include "PeachPainting.h"

void PeachPainting_EmitDestructor(PeachPainting *painting)
{
    painting->~PeachPainting();
}
