#ifndef DASPD_C_H
#define DASPD_C_H

#include "types.h"
#include "dActor_c.h"
#include "dExtAnmModel_c.h"
#include "dExtShadowModel_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daSpd_c_classInit  fBase_c::operator new(940 = 0x3ac), dActor_c::dActor_c(), stores _ZTV7daSpd_c,
 *                 then the four members below in this order.
 *   ~daSpd_c   the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x3ac is the factory's own literal, and the trailing byte fields close exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c (relocs.txt, ov071). Only the
 * slots declared below differ; every other slot holds the base's own word and is inherited,
 * so it is deliberately not redeclared here.
 *
 * THE FACTORY daSpd_c_classInit (historical alias Scuttlebug_Spawn) constructs the
 * object for the SPIDER registry profile at 0x02122c08.
 */

/* ROM RTTI: _ZTS7daSpd_c 0x02122bf0, _ZTI7daSpd_c 0x02122bfc, vtable
 * _ZTV7daSpd_c 0x02122c2c. d_a_spd.cpp emits all three and the manifest
 * compares them against the cartridge. */
struct daSpd_c : dActor_c {
    dExtAnmModel_c mAnmModel;                /* 0x0d4 */
    dExtShadowModel_c mShadowModel;                /* 0x138 */
    dCcAc_c mdCcAc_c;  /* 0x160 */
    dBgCh_Actr mWithMeshClsn;              /* 0x194 */
    /* Drop-shadow matrix. */
    Matrix4x3 mShadowMtx; /* 0x350 */
    /* Current row of data_ov071_02122fa8. Two pointer-to-member calls. */
    void *mStateRow;                         /* 0x380 */
    Vector3 mHome;                           /* 0x384 */
    /* Where the bug walks back to. Refreshed from mPos when it lands. */
    Vector3 mAnchor;                         /* 0x390 */
    s32 mState;                              /* 0x39c */
    /* Leap range: set from the distance to the player, then counted down. */
    s32 mLeapDist;                           /* 0x3a0 */
    s16 mLeapAngle;                          /* 0x3a4 */
    s16 mHomeAngleY;                         /* 0x3a6 */
    /* Counted down once per frame. Every read in this file is an unsigned halfword. */
    u16 mTimer;                              /* 0x3a8 */
    /* Coins paid by OnTurnIntoEgg and by the death poof. Init writes 3. */
    u8  mCoinCount;                          /* 0x3aa */
    u8  pad_3ab[0x1];

    /* Inline: mwcc emits D1 then D0 and no D2, which is the cartridge pair
       (0x0211f000, 0x0211f048). The body stays on this line so the header
       field walk still sees the brace. */
    virtual ~daSpd_c() {} /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual void OnTurnIntoEgg(daPly_c &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();

    /* State machine. mStateRow points at a {enter, run} pointer-to-member
       pair; the two trampolines call through it. */
    void SetState(int idx);
    void CallStateEnter();
    void RunState();
    int St_Wait_Init();
    int St_Wait_Main();
    int St_Drop_Init();
    int St_Drop_Main();
    int St_Walk_Init();
    int St_Walk_Main();
    int St_Leap_Init();
    int St_Leap_Main();
    int St_Recoil_Init();
    int St_Recoil_Main();
    int St_Return_Init();
    int St_Return_Main();
    int  func_ov071_0211fb0c();
    int  func_ov071_0211fa54();
    int  func_ov071_0211f8d0();
    int  func_ov071_0211f7d4();
    int  func_ov071_0211f6f8();
    int  func_ov071_0211f694();
    void UpdateModelAndShadow();
    void func_ov071_0211f498();
    void func_ov071_0211f29c();
    void func_ov071_0211f148(dBgCh_Actr *w);
    void func_ov071_0211f0b4();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Scuttlebug_size_must_be_0x3ac[sizeof(daSpd_c) == 0x3ac ? 1 : -1];
#endif

/* OnYoshiTryEat is the key function once ~daSpd_c is inline, so d_a_spd.cpp
 * emits _ZTV7daSpd_c (compared against the cartridge and discarded by the
 * manifest). The compiler emits the vtable from OnYoshiTryEat. */

#endif /* DASPD_C_H */
