// port/hal/smoke_dtor_face_actor.cpp -- the odr-use face that makes MSVC emit
// dBgActor_c's inline complete-object destructor, for smoke_actor,
// smoke_savestate and smoke_persist.
//
// Lane SMOKELINK5, run link100 wave 10 round 5, applying out/SMOKELINK4B/
// remaining.md section C, with the arity question that section left open
// settled below.
//
// port/hal/actorport_dtor_bridge.cpp is already on these three targets (it is
// the one host file SMOKELINK4B measured free) and its line 532 aliases
// ?Destructor1@dBgActor_c@@UAEXXZ onto ??1dBgActor_c@@QAE@XZ.  That right hand
// side is an INLINE, NON-VIRTUAL destructor (include/dBgActor_c.h's _MSC_VER
// arm: `~dBgActor_c() {}   /* no slot */`), so it is emitted only in a TU that
// odr-uses it -- the same fact port/hal/actorport_dtor_faces.cpp's header
// documents for its own eleven classes, none of which is dBgActor_c or
// dBase_c.
//
// ONLY dBgActor_c, NOT dBase_c.  remaining.md flagged this as "the obvious
// answer" without writing it, because it is the one row on the batch that is
// not a copy of an existing row for an existing class.  Measured before
// writing anything: out/SMOKELINK4B/remaining_37.txt lists
// ?Destructor1@dBgActor_c@@UAEXXZ and NOT ?Destructor1@dBase_c@@UAEXXZ, and
// the bridge's own alternatename rows for dActor_c and dBase_c
// (actorport_dtor_bridge.cpp:528, 530) point at ??1dActor_c@@QAE@XZ and
// ??1dBase_c@@QAE@XZ, which this link already defines -- dActor_c and dBase_c
// are both odr-used elsewhere on this batch (dActor_c's own destructor is on
// PORT_SMOKE_ACTOR_SRC, and dBase_c is its base). Only dBgActor_c's complete
// object destructor was never odr-used on these three targets, which is
// exactly the gap this one-class face closes.
//
// THE MECHANISM IS THE PARENT'S OWN MACRO, ACTORPORT_D1_FACE, applied to one
// class: `((dBgActor_c *)s)->~dBgActor_c();` inside a
// __fastcall(void *, void *) wrapper.  __fastcall(void*, void*) is the tree's
// dead-edx face shape: a __thiscall caller passes the receiver in ecx and
// neither side puts anything on the stack, so the alias the bridge already
// carries is ABI-exact once this TU exists to emit the body -- the same
// reasoning as hal's other faces and the arity audit in
// sm64ds-port-fastcall-face-arity.  The face's own name does not appear on the
// wall; only the destructor body it forces into existence does.

#include "dBgActor_c.h"

extern "C" void __fastcall smoke_actorport_d1_dBgActor_c(void *s, void *unused)
{
    (void)unused;
    ((dBgActor_c *)s)->~dBgActor_c();
}
