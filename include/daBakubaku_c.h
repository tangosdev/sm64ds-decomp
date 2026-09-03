#ifndef DABAKUBAKU_C_H
#define DABAKUBAKU_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

/* The last of the three intermediate bases the RTTI puts under dEnemyBase_c -- except
 * that this one has NO children, so it is only "intermediate" by where it sits in the
 * graph. The decomp had no name for it at all: every one of its functions was still
 * func_ov032_*, and the class is named here after the RTTI string, as #1400 did for
 * dScMgBase_c. That also makes the compiler's _ZTV12daBakubaku_c agree with
 * _ZTI12daBakubaku_c for free.
 *
 * TWO WITNESSES, and they agree offset for offset:
 *
 *   func_ov032_0211244c is the factory. fBase_c::operator new(0x438), then
 *   _ZN12dEnemyBase_cC2Ev, then ONE vtable store -- one, not two, which is what says nothing
 *   derives from this class -- then the five members below in this order.
 *
 *   func_ov032_021111a0 is the destructor: the same five backwards, then
 *   _ZN12dEnemyBase_cD2Ev. func_ov032_021111f0 is the same again plus Memory::Deallocate,
 *   so it is the deleting destructor.
 *
 * The vtable is at ov032 0x02113824 and the word before it is 0x021137e4, which is
 * _ZTI12daBakubaku_c. Note that ov027 has _ZTV10SlidingIce at the SAME address --
 * ov027 and ov032 overlap, so resolve inside ov032 or the table will answer for the
 * wrong overlay.
 *
 * SIZE 0x438, the factory's own literal. The members account for 0x110..0x3dc; four
 * fields of the 0x5c above that are now evidenced and named below, and the rest stays
 * padding rather than becoming invented fields.
 *
 * THE 4-BYTE "GAP" AT 0x3b0 IS NOT A GAP. ModelAnim is 0x64 and starts at 0x34c, so it
 * ends at 0x3b0 and both witnesses put ShadowModel at 0x3b4 -- but the word between
 * them is a live field, not alignment. Neither structor touches it, which is why the
 * two witnesses could not see it; Behavior reads it every frame. That is the general
 * shape of this correction: a ctor/dtor pair pins the MEMBERS, and only the ordinary
 * methods can pin the scalars between them.
 */
struct daBakubaku_c : dEnemyBase_c {
    dCcAcPos_c  mBodyClsn;       /* 0x110 */
    dCcAcPos_c  mHeadClsn;       /* 0x150 */
    dBgCh_Actr               mWithMeshClsn;   /* 0x190 */
    ModelAnim                  mModelAnim;      /* 0x34c */
    /* NOT padding after all. Behavior loads this WORD, reads a pointer-to-member
       out of the thing it points at and calls it on `this`, then compares the word
       itself against data_ov032_02113aac to pick a speed -- so it is the behaviour
       state the actor is currently in. Left void * on purpose: the table's element
       type is the incomplete-class pointer-to-member whose REPRESENTATION Behavior
       depends on, and completing it is a codegen change (see that file). */
    void                      *mState;          /* 0x3b0 */
    ShadowModel                mShadowModel;    /* 0x3b4 */
    u8                         pad_3dc[0x30];
    /* InitResources copies the actor's own position here once, and nothing else
       that is migrated writes it. */
    s32                        mSpawnPosX;      /* 0x40c */
    s32                        mSpawnPosY;      /* 0x410 */
    s32                        mSpawnPosZ;      /* 0x414 */
    u8                         pad_418[0x12];
    u16                        unk_42a;         /* 0x42a -- Behavior ticks it down */
    u8                         pad_42c[0xc];

    /* The slots it overrides, found by diffing all 31 against dEnemyBase_c's. All six bodies
       are now real methods -- Behavior and InitResources were the last two still
       written as free functions over raw offsets, and both reproduce unchanged. */
    virtual s32  InitResources();       /* slot  0 */
    virtual s32  CleanupResources();    /* slot  3 */
    virtual s32  Behavior();            /* slot  6 */
    virtual s32  Render();              /* slot  9 */
    virtual void OnPendingDestroy();    /* slot 12 */
    virtual int  OnAimedAtWithEgg();    /* slot 29 */

    /* DECLARED LAST on purpose. Nothing in the build DEFINES the key function, so
       where the virtual destructor is declared decides whether mwcc emits
       _ZTV/_ZTI/_ZTS at all: declared last it emits them, declared first it emits
       nothing and the validator's romdata ratchet reds the PR. */
    virtual ~daBakubaku_c();
};

typedef char daBakubaku_c_size_must_be_0x438[sizeof(daBakubaku_c) == 0x438 ? 1 : -1];

#endif /* DABAKUBAKU_C_H */
