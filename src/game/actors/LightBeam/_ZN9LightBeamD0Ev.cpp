//cpp
// @symbol _ZN9LightBeamD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "LightBeam.h"

void LightBeam_EmitDeletingDestructor(LightBeam *beam)
{
    delete beam;
}
