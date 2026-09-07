#ifndef CRAZEDCRATE_H
#define CRAZEDCRATE_H
#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* TWO WITNESSES:
 *
 *   daBttBk_c_classInit  fBase_c::operator new(888 = 0x378),
 *       dActor_c::dActor_c(), stores _ZTV11CrazedCrate, then the four
 *       members below in this order.
 *   _ZN11CrazedCrateD0Ev  the same four members destroyed in reverse,
 *       then ~dActor_c.
 *
 * SIZE 0x378 is the factory's own literal; unk_374 (4 bytes, 0x374) closes
 * exactly on it.
 *
 * Everything below 0x0d0 duplicated dActor_c's own fields under placeholder
 * names -- dActor_c ends at exactly 0x0d0. Two consumer fields were
 * repointed to inherited dActor_c names: unk_09c -> mVertAccel, unk_0a0 ->
 * mTerminalVelocity, unk_0b0 -> mFlags (mPosX/Y/Z and mScaleX/Y/Z already
 * shared dActor_c's names).
 *
 * mWithMeshClsn was mistyped `u8` at 0x180 in the generated header --
 * daBttBk_c_classInit calls _ZN10dBgCh_ActrC1Ev at that offset, so it is the
 * real 0x1bc-byte member (0x180..0x33c); the 0x38 bytes from 0x33c..0x374
 * are genuinely unevidenced padding.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. CrazedCrate
 * overrides slot 0 (InitResources), slot 3 (CleanupResources), slot 6
 * (Behavior) and slot 9 (Render) -- all still fBase_c's own slots in
 * dActor_c -- plus slot 12 (OnPendingDestroy, an extern "C" empty-body free
 * function), 18 (OnYoshiTryEat) and 19 (OnTurnIntoEgg). Every other slot
 * holds the base's own word and is inherited, so it is deliberately not
 * redeclared here.
 *
 * SM64DS RTTI names the implementation daBttBk_c. The reconstructed
 * factory daBttBk_c_classInit (historical alias
 * CrazedCrate_Spawn) constructs it for the BATTA_BLOCK
 * registry profile.
 */
struct CrazedCrate : dActor_c {
    u8  pad_0d0[0x4];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4. [_ZN11CrazedCrateD0Ev.c] */
    Model mModel;            /* 0x0d4 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x124. [_ZN11CrazedCrateD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x124 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x14c. [_ZN11CrazedCrateD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x14c */
    /* dBgCh_Actr member, named by daBttBk_c_classInit's own C1 call and the
       class's own destructor's D1 call at +0x180.
       [daBttBk_c_classInit.c, _ZN11CrazedCrateD0Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x180 */
    u8  pad_33c[0x38];
    s32 unk_374;            /* 0x374 */

    virtual ~CrazedCrate();            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  CleanupResources();      /* slot  3 */
    virtual s32  Behavior();         /* slot  6 */
    virtual s32  Render();           /* slot  9 */
    virtual void OnPendingDestroy();      /* slot 12 */
    virtual s32  OnYoshiTryEat();         /* slot 18 */
    virtual int  OnTurnIntoEgg(Player &player); /* slot 19 */
};

typedef char CrazedCrate_size_must_be_0x378[sizeof(CrazedCrate) == 0x378 ? 1 : -1];

#endif
