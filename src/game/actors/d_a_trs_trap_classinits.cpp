//cpp
/* Big Boo's Haunt animated-furniture traps: the mesh-collision callback and
 * the four factories -- ov063/daTrsTrap_c_classInit.
 *
 * This file is the whole linker unit 0x0211d270..0x0211d3a0, 6 functions:
 *   0x0211d270  LatchStandingActor  latches the player into mStandingActor
 *   0x0211d28c  OnMeshClsnTouch     the dBgW callback, forwarding to it
 *   0x0211d2a0  daTrsTrap_c_classInit_TERESAPIT, _MERRYGOROUND, _BOOKSHELF,
 *               _KAIDAN
 * Below it, the method TU (d_a_trs_trap.cpp) ends at 0x0211d270 with
 * InitResources. Above it, daObjTh_Fall_Block_c opens at 0x0211d3a0.
 *
 * KAIDAN, BOOKSHELF, MERRYGOROUND, and TERESAPIT all construct the ROM-proven
 * daTrsTrap_c class: each allocates 852 = 0x354, runs dActor_c's C2, stores
 * _ZTV11daTrsTrap_c, and constructs the dExtSimpleModel_c and dBgW_KcMbg members -- which
 * is what proves four spawn profiles of one class and not four classes. The
 * synthesized ctor from `return new` reproduces that sequence exactly, so the
 * hand-rolled operator-new/C2/vtable-store spelling is gone.
 *
 * InitResources installs OnMeshClsnTouch as mMovingMeshCollider's callback
 * word; it reaches the trap as its second argument and the colliding actor as
 * its third. OnMeshClsnTouch is a static member because the collider stores
 * its raw address; LatchStandingActor is an ordinary member. The ROM names
 * neither, so both names are descriptive.
 *
 * Reverse ROM order: without `#pragma defer_codegen off` mwcc emits the
 * functions last-defined first.
 *
 * NOTES:
 * - The synthesized ctor emits a vague-linkage _ZN9Matrix4x3D1Ev over
 *   mClsnMat (homeless; licensed deadstrip in this TU's manifest entry).
 * - LatchStandingActor keeps its int temporary: `if (actor->actorID == 0xbf)`
 *   does not match the ROM.
 */
#include "daTrsTrap_c.h"
#include "dBgW.h"

// @symbol daTrsTrap_c_classInit_KAIDAN
extern "C" daTrsTrap_c *daTrsTrap_c_classInit_KAIDAN(void)
{
    return new daTrsTrap_c();
}

// @symbol daTrsTrap_c_classInit_BOOKSHELF
extern "C" daTrsTrap_c *daTrsTrap_c_classInit_BOOKSHELF(void)
{
    return new daTrsTrap_c();
}

// @symbol daTrsTrap_c_classInit_MERRYGOROUND
extern "C" daTrsTrap_c *daTrsTrap_c_classInit_MERRYGOROUND(void)
{
    return new daTrsTrap_c();
}

// @symbol daTrsTrap_c_classInit_TERESAPIT
extern "C" daTrsTrap_c *daTrsTrap_c_classInit_TERESAPIT(void)
{
    return new daTrsTrap_c();
}

// @symbol _ZN11daTrsTrap_c15OnMeshClsnTouchEP4dBgWPS_P8dActor_c
void daTrsTrap_c::OnMeshClsnTouch(dBgW *clsn, daTrsTrap_c *self, dActor_c *actor) {
    self->LatchStandingActor(actor);
}

// @symbol _ZN11daTrsTrap_c18LatchStandingActorEP8dActor_c
void daTrsTrap_c::LatchStandingActor(dActor_c *actor) {
    int isPlayer = actor->actorID == 0xbf;
    if (isPlayer)
        mStandingActor = actor;
}
