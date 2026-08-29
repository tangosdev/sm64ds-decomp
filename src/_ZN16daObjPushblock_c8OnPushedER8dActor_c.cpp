//cpp
// @symbol _ZN16daObjPushblock_c8OnPushedER8dActor_c
/* recovered: named members + real C++ method */
/* daObjPushblock_c::OnPushed(dActor_c &) -- slot 25. Takes the pusher's facing as its
 * own slide direction and picks the speed from the pusher's state: 2 (a dash?)
 * gets the fast 0x8000, anything else 0x4000. */
#include "daObjPushblock_c.h"

/* THE MISSING RETURN IS DELIBERATE AND LOAD-BEARING. The header declares slot 25
 * as `int`, but this body sets no return value and the ROM's 0x2c bytes leave r0
 * holding whatever the caller had. Adding `return 0;` to satisfy the declaration
 * emits a real `mov r0,#0` and BREAKS THE MATCH -- measured 2026-08-22.
 *
 * Slot 25's return type is UNCONSTRAINED by evidence: this is the only recovered
 * OnPushed body in the tree, and a fall-off-the-end body compiles identically
 * under `int` and `void`. The `int` comes from the family convention and from
 * the neighbouring slots (OnAttacked1/2, OnHitFromUnderneath,
 * OnHitByCannonBlastedChar) which DO return values -- the same unfalsifiable
 * shape already analysed for slot 19, where four constrained overrides settle it.
 *
 * Harmless for the ROM build. NOT harmless for the host port: a C++ caller that
 * reads this result gets garbage. If the port ever branches on OnPushed's return,
 * that is the bug, and the fix is upstream in dActor_c.h's slot-25 return type --
 * not a `return 0;` here, which would desync the bytes. */
int daObjPushblock_c::OnPushed(dActor_c &other)
{
    char *r1 = (char *)&other;
    if (r1 == 0) return;
    mPrevAngleY = *(short *)(r1 + 0x8e);
    if (*(int *)(r1 + 8) == 2) mHorzSpeed = 0x8000;
    else mHorzSpeed = 0x4000;
}
