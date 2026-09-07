//cpp
// @symbol _ZN4ToadD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "Toad.h"

void Toad_EmitDeletingDestructor(Toad *toad)
{
    delete toad;
}
