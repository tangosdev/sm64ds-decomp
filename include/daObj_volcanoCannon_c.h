/* daObj_volcanoCannon_c, reconstructed from its factory, destructor pair, vtable/RTTI,
 * collision member, state dispatcher and five matched methods.
 *
 * The name is the cartridge's own: the ROM RTTI string at ov022:0x02114440
 * spells 21daObj_volcanoCannon_c, and the class was carried under the coined
 * name VolcanoFire until that string settled it. The registry profile is
 * OBJ_VOLCANO_CANNON, whose descriptor sits at 0x02114458.
 *
 * daObj_volcanoCannon_c_classInit allocates 0x11c bytes, constructs dActor_c, then dCcAc_c at
 * +0xd4. The D1 body destroys that member and then the base, closing the same
 * layout from the other direction.
 */
#ifndef DAOBJ_VOLCANOCANNON_C_H
#define DAOBJ_VOLCANOCANNON_C_H

#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

struct daObj_volcanoCannon_c : dActor_c {
    typedef int (daObj_volcanoCannon_c::*StateFunc)();

    struct State {
        StateFunc init;
        StateFunc behavior;
    };

    u8  pad_0d0[0x4];
    dCcAc_c mCylinderClsn;       /* 0x0d4 */
    State *mState;               /* 0x108 */
    dActor_c *mSpawner;          /* 0x10c */
    u16 mKillTimer;              /* 0x110 */
    u8  pad_112[0x2];
    u32 mParticleID;             /* 0x114 */
    s32 mKillPosY;               /* 0x118 */

    /* Inline is load-bearing: a forcing use in each destructor source emits
     * the ROM's D1/D0 pair without adding a homeless D2 to the enrolled file. */
    virtual ~daObj_volcanoCannon_c() {}

    virtual int InitResources();       /* slot 0 */
    virtual int CleanupResources();    /* slot 3 */
    virtual int Behavior();            /* slot 6 */

    int ChangeState(State *state);
};

typedef char daObj_volcanoCannon_c_size_must_be_0x11c[
    sizeof(daObj_volcanoCannon_c) == 0x11c ? 1 : -1];

#endif /* DAOBJ_VOLCANOCANNON_C_H */
