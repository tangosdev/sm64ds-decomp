#ifndef BABYPENGUIN_H
#define BABYPENGUIN_H
#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "MovingCylinderClsn.h"
#include "WithMeshClsn.h"

/* TWO WITNESSES:
 *
 *   BabyPenguin_Spawn  fBase_c::operator new(880 = 0x370),
 *       dActor_c::dActor_c(), stores _ZTV11BabyPenguin, then the four
 *       members below in this order.
 *   _ZN11BabyPenguinD0Ev  the same four members destroyed in reverse,
 *       then ~dActor_c.
 *
 * SIZE 0x370 is the factory's own literal; unk_36c (2 bytes, 0x36c) closes
 * exactly on it under 4-byte alignment.
 *
 * Everything below 0x0d0 duplicated dActor_c's own fields under placeholder
 * names -- dActor_c ends at exactly 0x0d0, so mEatingPlayer at 0x0d0 is
 * BabyPenguin's first own field. Two consumer fields were repointed to the
 * inherited dActor_c names: unk_09c -> mVertAccel, unk_0a0 ->
 * mTerminalVelocity (mPosX/Y/Z and mScaleX/Y/Z already shared dActor_c's
 * names, so needed no change).
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. BabyPenguin
 * overrides slot 0 (InitResources), slot 3 (CleanupResources), slot 6
 * (Behavior) and slot 9 (Render) -- all still fBase_c's own slots in
 * dActor_c -- plus slot 12 (OnPendingDestroy, an extern "C" empty-body free
 * function), 18 (OnYoshiTryEat) and 19 (OnTurnIntoEgg). Every other slot
 * holds the base's own word and is inherited, so it is deliberately not
 * redeclared here.
 */
struct BabyPenguin : dActor_c {
    s32 mEatingPlayer;            /* 0x0d0 */
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build checks. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x138. [_ZN11BabyPenguinD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x138 */
    /* MovingCylinderClsn member, named by the class's own destructor calling
       MovingCylinderClsn's D1 at +0x160. [_ZN11BabyPenguinD0Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x160 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x194. [_ZN11BabyPenguinD0Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x194 */
    s32 unk_350;            /* 0x350 */
    s32 unk_354;            /* 0x354 */
    s32 unk_358;            /* 0x358 */
    u8  pad_35c[0x4];
    s32 unk_360;            /* 0x360 */
    s32 unk_364;            /* 0x364 */
    u8  pad_368[0x4];
    s16 unk_36c;            /* 0x36c */

    virtual ~BabyPenguin();            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  CleanupResources();      /* slot  3 */
    virtual s32  Behavior();         /* slot  6 */
    virtual s32  Render();           /* slot  9 */
    virtual void OnPendingDestroy();      /* slot 12 */
    virtual s32  OnYoshiTryEat();         /* slot 18 */
    virtual int  OnTurnIntoEgg(Player &player); /* slot 19 */
};

typedef char BabyPenguin_size_must_be_0x370[sizeof(BabyPenguin) == 0x370 ? 1 : -1];

#endif
