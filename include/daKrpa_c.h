#ifndef DAKRPA_C_H
#define DAKRPA_C_H

#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"
#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* daKrpa_c is the ROM's own RTTI name for this class (this tree once coined it
 * FlameChomp): the typeinfo at ov070
 * 0x02123340 names dActor_c as the sole base at offset 0, and the class's
 * vtable at 0x02123370 (31 slots, same count as dActor_c's) is what pairs it
 * to daKrpa_c_classInit (historical aliases daKrpa_c_Spawn and
 * FlameChomp_Spawn). ov070 is mixed FLY_GUY/AMP/FLAME_CHOMP/FLAME_CHOMP_FIRE;
 * the debug table at 0x0208f8f8 names KERONPA (270), which overlay_actors.md
 * maps to FLAME_CHOMP. AMP is 266 / BIRIKYU / daBrq_c; flame-chomp fire is
 * 271 / KERONPA_FIRE / daKpFr_c.
 *
 * The factory is `return new daKrpa_c()`. The leaf unsigned-long operator new
 * forwards to `_ZN7fBase_cnwEj` until #2570's fBase overload lands.
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

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daKrpaState_size_must_be_0x10[
    sizeof(daKrpaState) == 0x10 ? 1 : -1];
#endif

struct daKrpaFrameController {
    u32 mode;
    u32 *frames;
    u32 count;
    u32 cursor;

    void func_ov070_02121ae0(u32 *frames, u32 count, u32 mode);
    u32  func_ov070_02121a64();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daKrpaFrameController_size_must_be_0x10[
    sizeof(daKrpaFrameController) == 0x10 ? 1 : -1];
#endif

struct daKrpa_c : dActor_c {
    u8                        pad_0d0[0x4];
    ModelAnim                 mModelAnim;                    /* 0x0d4 */
    dExtShadowModel_c               mShadowModel;                  /* 0x138 */
    dCcAcPos_c mdCcAcPos_c;    /* 0x160 */
    dBgCh_Actr              mWithMeshClsn;                 /* 0x1a0 */
    Matrix4x3                 mMatrix;                       /* 0x35c */
    daKrpaFrameController     mFrameController;              /* 0x38c */
    daKrpaState              *mStateMethods;                 /* 0x39c */
    s32                       mStateIndex;                   /* 0x3a0 */
    Player                   *mPlayer;                       /* 0x3a4 */
    /* InitResources raycasts a dBgCh_Gnd straight down from mPos and stores
       (mPosY - hit height) + 0x1e000, or the constant 0x1f4000 when nothing is
       hit. [_ZN8daKrpa_c13InitResourcesEv, now in src/game/actors/d_a_krpa.cpp] */
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

    void func_ov070_021211c4();
    void func_ov070_02121298();
    void func_ov070_02121310();
    int  func_ov070_021213cc();
    int  func_ov070_02121438();
    int  func_ov070_021214f8();
    int  func_ov070_02121548();
    int  func_ov070_0212156c();
    int  func_ov070_021216b8();
    int  func_ov070_02121710();
    int  func_ov070_021217ac();
    void func_ov070_0212180c();
    void func_ov070_02121848();
    void func_ov070_02121880(int state);

    static void *operator new(size_t size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daKrpa_c_size_must_be_0x3b0[
    sizeof(daKrpa_c) == 0x3b0 ? 1 : -1];
#endif

#endif /* DAKRPA_C_H */
