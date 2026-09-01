#ifndef DAKRPA_C_H
#define DAKRPA_C_H

#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"
#include "dActor_c.h"

/* daKrpa_c is the ROM's own RTTI name for this class (this tree once coined it
 * FlameChomp): the typeinfo at ov070
 * 0x02123340 names dActor_c as the sole base at offset 0, and the class's
 * vtable at 0x02123370 (31 slots, same count as dActor_c's) is what pairs it
 * to daKrpa_c_classInit (historical aliases daKrpa_c_Spawn and
 * FlameChomp_Spawn). A natural
 * `new daKrpa_c` is not retail-accurate: it targets the unresolved global
 * `_Znwm`, while the ROM calls fBase_c::operator new. The actor-table factory
 * therefore keeps an explicit typed construction seam for the allocator,
 * base/member constructors, and vptr store.
 *
 * The Spawn constructs the four owned subobjects below at 0xd4..0x1a0 in
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
struct daKrpa_c;
typedef void (daKrpa_c::*daKrpaStateMethod)();

struct daKrpaState {
    daKrpaStateMethod init;
    daKrpaStateMethod behavior;
};

typedef char daKrpaState_size_must_be_0x10[
    sizeof(daKrpaState) == 0x10 ? 1 : -1];

struct daKrpaFrameController {
    u32 mode;
    u32 *frames;
    u32 count;
    u32 cursor;
};

typedef char daKrpaFrameController_size_must_be_0x10[
    sizeof(daKrpaFrameController) == 0x10 ? 1 : -1];

struct daKrpa_c : dActor_c {
    u8                        pad_0d0[0x4];
    ModelAnim                 mModelAnim;                    /* 0x0d4 */
    ShadowModel               mShadowModel;                  /* 0x138 */
    dCcAcPos_c mdCcAcPos_c;    /* 0x160 */
    dBgCh_Actr              mWithMeshClsn;                 /* 0x1a0 */
    Matrix4x3                 mMatrix;                       /* 0x35c */
    daKrpaFrameController     mFrameController;              /* 0x38c */
    daKrpaState              *mStateMethods;                 /* 0x39c */
    s32                       mStateIndex;                   /* 0x3a0 */
    Player                   *mPlayer;                       /* 0x3a4 */
    /* InitResources raycasts a dBgCh_Gnd straight down from mPos and stores
       (mPosY - hit height) + 0x1e000, or the constant 0x1f4000 when nothing is
       hit. [_ZN8daKrpa_c13InitResourcesEv.cpp] */
    Fix12i                    mGroundDistance;                /* 0x3a8 */
    u8                        mStateTimer;                    /* 0x3ac */
    u8                        pad_3ad[0x3];

    /* Inline plus vtable instantiation is load-bearing: mwcc emits retail's
       D1 then D0 pair, with no homeless D2. InitResources is the first
       out-of-line virtual and anchors this TU's vtable/RTTI group. */
    virtual ~daKrpa_c() {}

    virtual s32  InitResources();       /* slot 0 */
    virtual s32  CleanupResources();    /* slot 3 */
    virtual s32  Behavior();            /* slot 6 */
    virtual s32  Render();              /* slot 9 */
    virtual void OnPendingDestroy();    /* slot 12 */
    virtual int  OnYoshiTryEat();       /* slot 18 */
};

typedef char daKrpa_c_size_must_be_0x3b0[
    sizeof(daKrpa_c) == 0x3b0 ? 1 : -1];

#endif /* DAKRPA_C_H */
