#ifndef DABAKUBAKU_C_H
#define DABAKUBAKU_C_H

#include "types.h"
#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsnWithPos.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

/* The last of the three intermediate bases the RTTI puts under dEnemyBase_c -- except
 * that this one has NO children, so it is only "intermediate" by where it sits in the
 * graph. The decomp had no name for it at all: every one of its functions was still
 * func_ov032_*, and the class is named here after the RTTI string, as #1400 did for
 * dScMgBase_c. That also makes the compiler's _ZTV12daBakubaku_c agree with
 * _ZTI12daBakubaku_c for free.
 *
 * TWO WITNESSES, and they agree offset for offset:
 *
 *   func_ov032_0211244c is the factory. ActorBase::operator new(0x438), then
 *   _ZN5EnemyC2Ev, then ONE vtable store -- one, not two, which is what says nothing
 *   derives from this class -- then the five members below in this order.
 *
 *   func_ov032_021111a0 is the destructor: the same five backwards, then
 *   _ZN5EnemyD2Ev. func_ov032_021111f0 is the same again plus Memory::Deallocate,
 *   so it is the deleting destructor.
 *
 * The vtable is at ov032 0x02113824 and the word before it is 0x021137e4, which is
 * _ZTI12daBakubaku_c. Note that ov027 has _ZTV10SlidingIce at the SAME address --
 * ov027 and ov032 overlap, so resolve inside ov032 or the table will answer for the
 * wrong overlay.
 *
 * SIZE 0x438, the factory's own literal. The members account for 0x110..0x3dc and the
 * 0x5c above that is this class's own, still unevidenced: none of its eight functions
 * touches it through anything but a raw offset, so it stays padding rather than
 * becoming invented fields.
 *
 * THE 4-BYTE GAP AT 0x3b0 IS REAL, not a mistake: ModelAnim is 0x64 and starts at
 * 0x34c, so it ends at 0x3b0, but both witnesses put ShadowModel at 0x3b4.
 */
struct daBakubaku_c : Enemy {
    MovingCylinderClsnWithPos  mBodyClsn;       /* 0x110 */
    MovingCylinderClsnWithPos  mHeadClsn;       /* 0x150 */
    WithMeshClsn               mWithMeshClsn;   /* 0x190 */
    ModelAnim                  mModelAnim;      /* 0x34c */
    u8                         pad_3b0[0x4];
    ShadowModel                mShadowModel;    /* 0x3b4 */
    u8                         pad_3dc[0x5c];

    virtual ~daBakubaku_c();

    /* The eight slots it overrides, found by diffing all 31 against Enemy's. The two
       still spelt as free functions in their own files are Behavior and
       InitResources; they are named here because the vtable names them, not because
       the bodies have been migrated yet. */
    virtual s32  InitResources();       /* slot  0 */
    virtual s32  CleanupResources();    /* slot  3 */
    virtual s32  Behavior();            /* slot  6 */
    virtual s32  Render();              /* slot  9 */
    virtual void OnPendingDestroy();    /* slot 12 */
    virtual int  OnAimedAtWithEgg();    /* slot 29 */
};

typedef char daBakubaku_c_size_must_be_0x438[sizeof(daBakubaku_c) == 0x438 ? 1 : -1];

#endif /* DABAKUBAKU_C_H */
