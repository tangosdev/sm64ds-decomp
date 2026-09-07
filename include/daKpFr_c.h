#ifndef DAKPFR_C_H
#define DAKPFR_C_H

#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"
#include "dActor_c.h"

/* daKpFr_c is the ROM's own RTTI name for this class (this tree once coined it
 * FlameChompFire): the typeinfo at ov070
 * 0x02123418 names dActor_c as the sole base at offset 0, and the class's
 * vtable at 0x02123448 (31 slots, same count as dActor_c's) is what pairs it
 * to daKpFr_c_classInit (historical aliases daKpFr_c_Spawn and
 * FlameChompFire_Spawn).
 * A natural `new daKpFr_c` was measured and rejected: its instruction bytes
 * match after relocation masking, but it targets the unresolved global
 * `_Znwm` rather than retail's fBase_c::operator new. The actor-table factory
 * therefore keeps an explicit typed construction seam for the allocator,
 * base/member constructors, and vptr store.
 *
 * The Spawn constructs the three owned subobjects below at 0xd4..0x130 in
 * declaration order; D1 destroys them in exactly the reverse order before
 * chaining to dActor_c::~dActor_c -- two independent witnesses for each
 * member's type and offset.
 *
 * Own vtable slots, from the ROM table diffed against dActor_c's: 0
 * InitResources, 3 CleanupResources, 6 Behavior, 9 Render, 12
 * OnPendingDestroy, 16/17 the destructor pair, 18 OnYoshiTryEat. Every other
 * slot is inherited.
 *
 * The header this replaces was deliberately flat -- a non-deriving struct
 * whose leading "fields" duplicated dActor_c's storage -- because giving a
 * non-derived struct a virtual would have inserted a vptr and shifted every
 * offset. Deriving from dActor_c is what makes the declarations below honest.
 */
struct daKpFr_c;
typedef void (daKpFr_c::*daKpFrStateMethod)();

struct daKpFrState {
    daKpFrStateMethod init;
    daKpFrStateMethod behavior;
};

typedef char daKpFrState_size_must_be_0x10[
    sizeof(daKpFrState) == 0x10 ? 1 : -1];

struct daKpFr_c : dActor_c {
    u8                 pad_0d0[0x4];
    ShadowModel        mShadowModel;           /* 0x0d4 */
    dCcAc_c mdCcAc_c;    /* 0x0fc */
    dBgCh_Actr       mWithMeshClsn;          /* 0x130 */
    /* InitResources assigns IDENTITY_MATRIX4X3 into this slot, Render passes
       it to DropShadowRadHeight, and its translation lives at 0x310. */
    Matrix4x3          mMatrix;                /* 0x2ec */
    daKpFrState       *mStateMethods;          /* 0x31c */
    s32                mStateResult;           /* 0x320 */
    /* Two particle handles, effects 0x7f and 0x80, both fed back into
       Particle::System::NewUnkCallback818 every Render, at mPosY + 0x4b000.
       [_ZN8daKpFr_c6RenderEv.cpp] */
    u32                mParticle1;             /* 0x324 */
    u32                mParticle2;             /* 0x328 */
    u8                 mStateTimer;            /* 0x32c */
    u8                 pad_32d[0x3];

    /* Inline plus vtable instantiation is load-bearing: mwcc emits retail's
       D1 then D0 pair, with no homeless D2. InitResources is the first
       out-of-line virtual and anchors this TU's vtable/RTTI group. */
    virtual ~daKpFr_c() {}

    virtual s32  InitResources();       /* slot 0 */
    virtual s32  CleanupResources();    /* slot 3 */
    virtual s32  Behavior();            /* slot 6 */
    virtual s32  Render();              /* slot 9 */
    virtual void OnPendingDestroy();    /* slot 12 */
    virtual int  OnYoshiTryEat();       /* slot 18 */
};

typedef char daKpFr_c_size_must_be_0x330[
    sizeof(daKpFr_c) == 0x330 ? 1 : -1];

#endif /* DAKPFR_C_H */
