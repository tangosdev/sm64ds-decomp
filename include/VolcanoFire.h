/* VolcanoFire, reconstructed from its factory, destructor pair, vtable/RTTI,
 * collision member, state dispatcher and five matched methods.
 *
 * The cartridge's RTTI calls this class daObj_volcanoCannon_c while the
 * configured function names use VolcanoFire. The English spelling is kept as
 * the compatibility-facing C++ class; the ROM-owned metadata is audited
 * independently at _ZTI21daObj_volcanoCannon_c / _ZTV21daObj_volcanoCannon_c.
 *
 * VolcanoFire_Spawn allocates 0x11c bytes, constructs dActor_c, then dCcAc_c at
 * +0xd4. The D1 body destroys that member and then the base, closing the same
 * layout from the other direction.
 */
#ifndef VOLCANOFIRE_H
#define VOLCANOFIRE_H

#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

struct VolcanoFire : dActor_c {
    typedef int (VolcanoFire::*StateFunc)();

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
    virtual ~VolcanoFire() {}

    virtual int InitResources();       /* slot 0 */
    virtual int CleanupResources();    /* slot 3 */
    virtual int Behavior();            /* slot 6 */

    int ChangeState(State *state);
};

typedef char VolcanoFire_size_must_be_0x11c[
    sizeof(VolcanoFire) == 0x11c ? 1 : -1];

#endif /* VOLCANOFIRE_H */
