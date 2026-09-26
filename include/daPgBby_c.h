#ifndef DAPGBBY_C_H
#define DAPGBBY_C_H
#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

struct Player;

/* PENGUIN_BABY -- ov072/daPgBby_c, the lost baby penguin of Cool, Cool
 * Mountain that the player carries back to its mother.
 *
 * IDENTITY. ROM RTTI names the class: _ZTS9daPgBby_c at 0x02122a54, and the
 * __si_class_type_info at 0x02122a60 names dActor_c as its single base. The
 * vtable's address point is 0x02122a90 and it is exactly dActor_c's 31
 * slots long. The PENGUIN_BABY profile at 0x02122a6c relocates to
 * daPgBby_c_classInit.
 *
 * SIZE 0x370 is the factory's own fBase_c::operator new literal. The
 * factory constructs the four typed members below in declaration order and
 * the destructor pair tears them down in reverse.
 *
 * STATE MACHINE. mState points at one of six {enter, update} member-pointer
 * pairs in overlay .bss (built by this overlay's static initializer);
 * mStateId mirrors the index and mSubState is the per-state step counter.
 *
 *   0  func_ov072_02121c94 / func_ov072_02121a84
 *   1  func_ov072_02121a28 / func_ov072_021218dc
 *   2  func_ov072_02121890 / func_ov072_021217ac
 *   3  func_ov072_02121758 / func_ov072_02121670
 *   4  func_ov072_02121640 / func_ov072_021214dc
 *   5  func_ov072_02121368 / func_ov072_021212c0
 *
 * (pairs read from the ROM member-pointer literals at 0x021229f4..0x02122a54
 * and the static initializer that copies them into the table).
 *
 * VTABLE. daPgBby_c overrides dActor_c slots 0 (InitResources), 3
 * (CleanupResources), 6 (Behavior), 9 (Render), 12 (OnPendingDestroy),
 * 16/17 (the destructor pair), 18 (OnYoshiTryEat) and 19 (OnTurnIntoEgg);
 * every other slot holds dActor_c's own word.
 */
struct daPgBby_c : dActor_c {
    /* The player whose Yoshi has the penguin in its mouth; state 5's enter
       step places the penguin in front of it and clears it. */
    Player *mEatingPlayer;          /* 0x0d0 */
    ModelAnim mModelAnim;           /* 0x0d4 */
    ShadowModel mShadowModel;       /* 0x138 */
    dCcAc_c mCylClsn;               /* 0x160 */
    dBgCh_Actr mWithMeshClsn;       /* 0x194 */
    /* Copy of mPosX/Y/Z taken once in InitResources and never written
       again; the respawn check teleports back here. */
    s32 mSpawnPosX;                 /* 0x350 */
    s32 mSpawnPosY;                 /* 0x354 */
    s32 mSpawnPosZ;                 /* 0x358 */
    void *mState;                   /* 0x35c -- current {enter, update} pair */
    Player *mCarrier;               /* 0x360 -- the player holding it */
    /* Lazily filled with FindWithActorID(0x101, 0) and never cleared.
       Actor 0x101 (257) is PENGUIN_MOTHER (notes/ead-debug-name-crossref.md). */
    dActor_c *mMother;              /* 0x364 */
    s32 mStateId;                   /* 0x368 */
    u16 mRespawnTimer;              /* 0x36c */
    u8  mSubState;                  /* 0x36e */

    virtual s32  InitResources();               /* slot  0 */
    virtual s32  CleanupResources();            /* slot  3 */
    virtual s32  Behavior();                    /* slot  6 */
    virtual s32  Render();                      /* slot  9 */
    virtual void OnPendingDestroy();            /* slot 12 */
    virtual s32  OnYoshiTryEat();               /* slot 18 */
    virtual void OnTurnIntoEgg(Player &player); /* slot 19 */

    /* Declared last and inline so class instantiation emits the retail D1/D0
       pair in that order, with no separate D2 body. */
    virtual ~daPgBby_c() {}                     /* slots 16 (D1), 17 (D0) */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daPgBby_c_size_must_be_0x370[sizeof(daPgBby_c) == 0x370 ? 1 : -1];
#endif

#endif
