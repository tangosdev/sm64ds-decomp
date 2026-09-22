//cpp
// @symbol _ZN9KoopaFlagD0Ev
#include "KoopaFlag.h"

/* A delete expression forces the compiler-spelled deleting destructor. */
void KoopaFlag_EmitDeletingDestructor(KoopaFlag *flag)
{
    delete flag;
}
