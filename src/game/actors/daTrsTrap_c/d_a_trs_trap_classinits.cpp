//cpp
/* Big Boo's Haunt animated-furniture traps: the four factories -- ov063.
 *
 * KAIDAN, BOOKSHELF, MERRYGOROUND, and TERESAPIT all construct the ROM-proven
 * daTrsTrap_c class: each allocates 852 = 0x354, runs dActor_c's C2, stores
 * _ZTV11daTrsTrap_c, and constructs the Model and dBgW_KcMbg members -- which
 * is what proves four spawn profiles of one class and not four classes. The
 * synthesized ctor from `return new` reproduces that sequence exactly, so the
 * hand-rolled operator-new/C2/vtable-store spelling is gone.
 *
 * SPLIT from d_a_trs_trap.cpp (see its header): the collision-callback pair
 * at 0x0211d270..0x0211d2a0 sits between InitResources and these factories,
 * and one delinks entry licenses one contiguous range. Reverse ROM order, as
 * there. C LINKAGE IS LOAD-BEARING -- the ROM symbols are the bare names.
 *
 * deslop leftovers:
 * - The synthesized ctor emits a vague-linkage _ZN9Matrix4x3D1Ev over
 *   mClsnMat (homeless; licensed deadstrip in this TU's manifest entry).
 */
#include "daTrsTrap_c.h"

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
