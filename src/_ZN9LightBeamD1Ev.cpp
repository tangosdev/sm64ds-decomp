//cpp
// @symbol _ZN9LightBeamD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
#include "LightBeam.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D1 NAME, AND MSVC NEVER EMITS IT. ~LightBeam()
 * is defined in the class body, and MSVC emits an inline destructor only for a
 * caller that odr-uses it. The port's slot 16 for LIGHT_BEAM (tr_d1 in
 * hal/actor_classes.cpp, the id-0x25 branch of the table TRAP shares) calls
 * the flat name, so this arm defines it as the qualified, direct call that
 * makes MSVC emit the body, the shape port/tools/dtorfwd.py generates for
 * every inline-destructor class. QUALIFIED on purpose: `self->~LightBeam()`
 * would be a virtual call through the port's table at slot 16, straight back
 * into this symbol. The inline chain MSVC emits stores an MSVC vftable where
 * the cartridge's D1 (0x021119d0, ov010) stores _ZTV9LightBeam;
 * port/tools/dtor_store_guard.py reads this arm (SRC_ARMS) and re-proves from
 * the ROM body on every build that nothing dispatches before the base
 * destructor restores a ROM-shaped table. Nothing here reaches mwccarm: it
 * builds the `#else` arm and emits the ROM bytes it always emitted. */
extern "C" void _ZN9LightBeamD1Ev(void *self)
{ ((LightBeam *)self)->LightBeam::~LightBeam(); }
#else
void LightBeam_EmitDestructor(LightBeam *beam)
{
    beam->~LightBeam();
}
#endif
