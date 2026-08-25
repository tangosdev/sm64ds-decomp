// Gate-7-only constructor/destructor bridge for Animation.
//
// Separate from hal/anim_bridge.cpp, which forwards through a LOCAL shadow
// `struct Animation` -- including the real include/Animation.h there would be
// a type redefinition. Separate from hal/ctor_bridge.cpp because only the
// gate-7 targets slice Animation, and naming it from the shared model-family
// bridge would leave smoke_model with an unresolved reference to a class it
// never builds.
//
// See hal/ctor_bridge.cpp for the C1/C2 collapse this exists to absorb.
#include "common.h"

#include <new>

#include "Animation.h"

extern "C" {
void _ZN9AnimationD1Ev(void *self);

/* C2 is the base-subobject variant of the C1 that gate 7 builds, with the
   identical body. MSVC has one constructor symbol, so only C1 is compiled and
   the C2 name is defined here. */
void _ZN9AnimationC2Ev(void *self) { ::new (self) Animation(); }
}

/* The destructor is still a flat extern "C" Itanium function in src/, so this
   bridge runs the other way -- MSVC's vtable slot 0 reaches the ROM body. */
Animation::~Animation() { _ZN9AnimationD1Ev(this); }

#include "ModelAnim.h"

// ModelAnim, same shape as Model in hal/ctor_bridge.cpp: the port dispatches
// it through the synthetic _ZTV9ModelAnim that hal/model_host.cpp provides and
// src/_ZN9ModelAnimC2Ev.c installs, so the constructor runs for its member
// initialization and the vptr goes back to the table the port dispatches
// through rather than the one placement new leaves.
extern "C" void *_ZTV9ModelAnim[10];
extern "C" void _ZN9ModelAnimC1Ev(void *self)
{
    ::new (self) ModelAnim();
    *(void **)self = _ZTV9ModelAnim;
}
