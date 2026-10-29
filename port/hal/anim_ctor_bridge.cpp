// Gate-7-only constructor bridge for dExtFrameCtrl_c.
//
// Separate from hal/anim_bridge.cpp, which forwards through a LOCAL shadow
// `struct dExtFrameCtrl_c` -- including the real include/dExtFrameCtrl_c.h there would be
// a type redefinition. Separate from hal/ctor_bridge.cpp because only the
// gate-7 targets slice dExtFrameCtrl_c, and naming it from the shared model-family
// bridge would leave smoke_model with an unresolved reference to a class it
// never builds.
//
// See hal/ctor_bridge.cpp for the C1/C2 collapse this exists to absorb.
#include "common.h"

#include <new>

#include "dExtFrameCtrl_c.h"

extern "C" {
/* C2 is the base-subobject variant of the C1 that gate 7 builds, with the
   identical body. MSVC has one constructor symbol, so only C1 is compiled and
   the C2 name is defined here. */
void _ZN15dExtFrameCtrl_cC2Ev(void *self) { ::new (self) dExtFrameCtrl_c(); }
}

#include "dExtAnmModel_c.h"

// dExtAnmModel_c, same shape as dExtSimpleModel_c in hal/ctor_bridge.cpp: the port dispatches
// it through the synthetic _ZTV14dExtAnmModel_c that hal/model_host.cpp provides and
// dExtAnmModel_c::dExtAnmModel_c() in src/engine/model/d_ext_model.cpp installs, so the
// constructor runs for its member initialization and the vptr goes back to
// the table the port dispatches through rather than the one placement new
// leaves.
extern "C" void *_ZTV14dExtAnmModel_c[10];
extern "C" void _ZN14dExtAnmModel_cC1Ev(void *self)
{
    ::new (self) dExtAnmModel_c();
    *(void **)self = _ZTV14dExtAnmModel_c;
}
