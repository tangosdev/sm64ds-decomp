#ifndef POWERFLOWER_H
#define POWERFLOWER_H
#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* TWO WITNESSES:
 *
 *   PowerFlower_Spawn  fBase_c::operator new(972 = 0x3cc),
 *       dActor_c::dActor_c(), stores _ZTV11PowerFlower, then the five
 *       members below in this order.
 *   _ZN11PowerFlowerD0Ev  the same five members destroyed in reverse,
 *       then ~dActor_c.
 *
 * SIZE 0x3cc is the factory's own literal; mLifeTimer (1 byte, 0x3ca) closes
 * exactly on it under 4-byte alignment.
 *
 * Everything below 0x0d0 duplicated dActor_c's own fields under placeholder
 * names -- dActor_c ends at exactly 0x0d0. Consumer fields were repointed to
 * the inherited dActor_c/fBase_c names: mParam -> param1, unk_08e ->
 * mAngleY, unk_09c -> mVertAccel, unk_0a0 -> mTerminalVelocity, unk_0b0 ->
 * mFlags (mPosX/Y/Z and mScaleX/Y/Z already shared dActor_c's names).
 *
 * mShadowModel was mistyped `u8` at 0x174 in the generated header --
 * PowerFlower_Spawn calls _ZN11ShadowModelC1Ev at that offset, so it is the
 * real 0x28-byte member (0x174..0x19c); the 0x30 bytes from 0x19c..0x1cc
 * are genuinely unevidenced padding.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. PowerFlower
 * overrides slot 0 (InitResources), slot 3 (CleanupResources), slot 6
 * (Behavior) and slot 9 (Render) -- all still fBase_c's own slots in
 * dActor_c -- plus slot 18 (OnYoshiTryEat). Every other slot holds the
 * base's own word and is inherited, so it is deliberately not redeclared
 * here.
 */
struct PowerFlower : dActor_c {
    u8  pad_0d0[0x4];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks. */
    Model mModel1;            /* 0x0d4 */
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x124. [_ZN11PowerFlowerD0Ev.c] */
    Model mModel2;            /* 0x124 */
    /* ShadowModel member, named by PowerFlower_Spawn's own C1 call and the
       class's own destructor's D1 call at +0x174.
       [PowerFlower_Spawn.c, _ZN11PowerFlowerD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x174 */
    u8  pad_19c[0x30];
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x1cc. [_ZN11PowerFlowerD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x1cc */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x200. [_ZN11PowerFlowerD0Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x200 */
    /* The ground height under the flower: InitResources raycasts a dBgCh_Gnd
       from (mPos with Y + 0x14000) and stores the hit height (+0x44 of the
       ground object), falling back to that probe Y when nothing is hit.
       [_ZN11PowerFlower13InitResourcesEv.cpp] */
    s32 mGroundY;            /* 0x3bc */
    /* Render switches on it to pick which model to draw: 0 -> mModel1,
       1 and 2 -> mModel2. [_ZN11PowerFlower6RenderEv.cpp] */
    s32 mState;            /* 0x3c0 */
    u8  pad_3c4[0x6];
    /* Seeded 0xb4 (180 frames, three seconds) in InitResources. Render skips
       drawing on odd values once it is below 0x2d, so the flower blinks through
       its last 45 frames -- the standard "about to disappear" tell.
       [_ZN11PowerFlower13InitResourcesEv.cpp, _ZN11PowerFlower6RenderEv.cpp] */
    u8  mLifeTimer;            /* 0x3ca */

    virtual ~PowerFlower();            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  CleanupResources();      /* slot  3 */
    virtual s32  Behavior();         /* slot  6 */
    virtual s32  Render();           /* slot  9 */
    virtual s32  OnYoshiTryEat();         /* slot 18 */
};

typedef char PowerFlower_size_must_be_0x3cc[sizeof(PowerFlower) == 0x3cc ? 1 : -1];

#endif
