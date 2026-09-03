// @symbol func_ov072_02120874
// recovered name: daBgSnwmn_c_D0
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daBgSnwmn_c::~daBgSnwmn_c (D0, the deleting destructor). The mislabel
 * "OnYoshiTryEat" and its prior "verified" claim were both stale: t[0] was
 * assigned from `VT0`, decl_common.h's generic `int VT0[20]` shared-header
 * placeholder (port/hal/actor_vtables.cpp), not this class's own table --
 * it compiles and even byte-matches (match.py wildcards the literal-pool
 * word a vtable store loads, so it cannot see which symbol filled the
 * slot), but on the port it silently stores a 20-int dummy array's address
 * over the object's own vtable instead of _ZTV11daBgSnwmn_c, and no
 * per-file COMPILE_DEFINITIONS override in port/CMakeLists.txt redirected
 * it for this file. That is an unfinished recovery, which is why this file
 * carried no `complete` marker in config/arm9/overlays/ov072/delinks.txt
 * and dsd supplied its bytes straight from the ROM instead of building it.
 * Fixed to the same shape D1 (func_ov072_02120824.c, sibling in this pair)
 * already proves: own vtable store, then the five members in reverse
 * declaration order (dCcAcPos_c/MovingCylinderClsnWithPos-shaped @0x1b0,
 * ShadowModel @0x188, TextureSequence @0x174, Model @0x124, Model @0xd4),
 * then Actor's D2, then the deleting destructor's own Memory::Deallocate
 * epilogue. */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN15TextureSequenceD1Ev(void *);
extern void *G0;
int *func_ov072_02120874(int *t)
{
    t[0] = (int)_ZTV11daBgSnwmn_c;
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x1b0);
    _ZN11ShadowModelD1Ev((char *)t + 0x188);
    _ZN15TextureSequenceD1Ev((char *)t + 0x174);
    _ZN5ModelD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
