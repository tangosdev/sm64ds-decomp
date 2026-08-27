//cpp
// @symbol _ZN9LightBeamD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
#include "LightBeam.h"

void LightBeam_EmitDestructor(LightBeam *beam)
{
    beam->~LightBeam();
}
