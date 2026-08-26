//cpp
// @symbol _ZN13PrincessPeachD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "PrincessPeach.h"

void PrincessPeach_EmitDeletingDestructor(PrincessPeach *peach)
{
    delete peach;
}
