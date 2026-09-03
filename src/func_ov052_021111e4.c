// @symbol func_ov052_021111e4
// recovered name: daObjEmmLog_c_D0
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daObjEmmLog_c::~daObjEmmLog_c (D0, the deleting destructor) - the mislabel
 * "OnYoshiTryEat" and the prior body were both an unfinished auto-generated
 * stub: `t[0] = VT0; t[0] = VT1;` used decl_common.h's generic `int VT0[20]`
 * / `int VT1[20]` shared-header placeholders (port/hal/actor_vtables.cpp),
 * not this class's own tables. It compiles and even byte-matches (match.py
 * wildcards the literal-pool word a vtable store loads, so it cannot see
 * which symbol filled the slot), but on the port it silently stored a
 * 20-int dummy array's address over the object's own vtable and dBgActor_c's
 * instead of _ZTV13daObjEmmLog_c and _ZTV10dBgActor_c, and no per-file
 * COMPILE_DEFINITIONS override in port/CMakeLists.txt redirected either for
 * this file. That is an unfinished recovery, which is why this file carried
 * no `complete` marker in config/arm9/overlays/ov052/delinks.txt and dsd was
 * supplying its bytes straight from the ROM instead of building it. Fixed
 * to the same shape D1 (func_ov052_021111a0.c, already complete and
 * matched) already proves: own vtable store, then dBgActor_c's, then the
 * member chain in reverse declaration order (MovingMeshCollider @0x124,
 * Model @0xd4), then Actor's D2, then the deleting destructor's own
 * Memory::Deallocate epilogue. */
extern void *G0;
int *func_ov052_021111e4(int *t)
{
    t[0] = (int)_ZTV13daObjEmmLog_c;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
