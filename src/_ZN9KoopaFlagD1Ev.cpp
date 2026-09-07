//cpp
// @symbol _ZN9KoopaFlagD1Ev
#include "KoopaFlag.h"

/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
void KoopaFlag_EmitDestructor(KoopaFlag *flag)
{
    flag->~KoopaFlag();
}
