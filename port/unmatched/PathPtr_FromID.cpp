/* FLAT-NAME BRIDGE for src/_ZN7PathPtr6FromIDEj.cpp -- PathPtr::FromID(unsigned)
 * is now a real matched C++ member (mangles to ?FromID@PathPtr@@QAEXI@Z under
 * MSVC). About 28 unique object files across the port -- RollingIronBall,
 * MetalNetLift, daMky_c, daMip_c, Player, ToxBox, SkiLift, Klepto,
 * RacingPenguin, UnchainedChomp, Unagi, MrBlizzard, Shark,
 * RotatingUpDownPlatform, BigMovingIceBlock, KoopaTheQuick, level_boot and
 * several func_ov0** actor bodies -- call the FLAT cdecl symbol
 * _ZN7PathPtr6FromIDEj directly and never go through the C++ member, so the
 * flat name has to keep existing (PATHPTR1 measured LNK1120 unresolved
 * externals across exactly this set when the flat body was deleted instead).
 *
 * This file used to hold FromID's body directly, as a register-ride-through
 * host copy (see ORIGINAL REASONING below); that reason went stale once the
 * matched TU (src/_ZN7PathPtr6FromIDEj.cpp) started building for the host as
 * a real __thiscall member and collided with it (LNK2005 against
 * unmatched/Klepto_PathPtrFaces.cpp's forwarder, now retired). The flat
 * symbol below is now just a receiver cast into that member -- the same
 * PORT_HOST_ABI recipe as hal/method_faces.cpp's _ZNK7PathPtr8NumNodesEv.
 * NOT an /alternatename: the flat callers are cdecl with self on the stack,
 * the member is __thiscall with self in ECX -- aliasing the two delivers the
 * receiver in the wrong place (sm64ds-port-fastcall-face-arity.md).
 *
 * ORIGINAL REASONING (kept for the record): PathPtr is two words, and mwcc
 * returns it through a caller-supplied slot in r0 with the id in r1. FromID's
 * whole body is a call to func_0203ad6c, whose matched source is `p[0] = v`
 * -- the slot in r0, the value in r1 -- and FromID names only the value,
 * letting the return slot ride through in r0. MSVC returns an eight-byte
 * struct in EAX:EDX instead, so a matched pair compiled straight for the
 * host reads the callers' `thiz` as the id and writes the record address
 * over whatever the first stack argument happened to be -- the
 * register-ride-through mismatch this host copy used to work around by
 * hand. The matched member now does the equivalent store itself
 * (self->def = data_020a0d84 + id * 6, via func_0203ad6c), so the receiver
 * cast below is all that is needed.
 */
#include "PathPtr.h"

extern "C" {

/* PORT_HOST_ABI: __thiscall receiver in ECX, delivered here from the cdecl
 * flat name's explicit self argument on the stack. See the header. */
void _ZN7PathPtr6FromIDEj(void *self, unsigned id)
{
    ((PathPtr *)self)->PathPtr::FromID(id);
}

}
