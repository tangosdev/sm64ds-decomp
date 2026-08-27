//cpp
// @symbol _ZN13PrincessPeachD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
#include "PrincessPeach.h"

void PrincessPeach_EmitDestructor(PrincessPeach *peach)
{
    peach->~PrincessPeach();
}
