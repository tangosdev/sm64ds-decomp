// @symbol func_ov052_021111e4
// recovered name: daObjEmmLog_c_D0
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daObjEmmLog_c::~daObjEmmLog_c (D0, the deleting destructor) - the mislabel
 * "OnYoshiTryEat" and the two undefined VT0/VT1 placeholders were an
 * unfinished auto-generated stub; that is why this file carried no
 * `complete` marker in config/arm9/overlays/ov052/delinks.txt and dsd was
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
