#ifndef HOOTTHEOWL_H
#define HOOTTHEOWL_H

#include "types.h"

/* Derives from dEnemyBase_c, and the class's own destructor is what proves it:
 * `_ZN10HootTheOwlD1Ev` stores this vtable, destroys the four members below in
 * reverse declaration order, then calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header
 * used to restate below 0x110 belongs to that chain and is inherited now --
 * nine of those markers turned out to be named dActor_c/dEnemyBase_c fields (mAngleX,
 * mPrevAngleX, mVertAccel, mTerminalVelocity, mVertSpeed, ...).
 *
 * THE FOUR MEMBERS CLOSE EXACTLY ON EACH OTHER, which is four independent
 * confirmations of one layout rather than four guesses:
 *
 *     0x110 dCcAcPos_c  0x40   -> 0x150
 *     0x150 dBgCh_Actr               0x1bc  -> 0x30c
 *     0x30c ModelAnim                  0x64   -> 0x370
 *     0x370 ShadowModel                0x28   -> 0x398
 *
 * and dEnemyBase_c's own 0x110 closes exactly on the first of them.
 *
 * Typing them absorbed four markers that were their insides, each corroborated
 * by what the code does with it:
 *   - unk_128 = mdCcAcPos_c.flags  (dCc_c +0x18)
 *   - mAnimation = the ModelAnim's Animation base (+0x50)
 *   - unk_364 = that Animation's currFrame (+0x08); Behavior reads it as
 *     `>> 12`, i.e. the integer frame of a 20.12 fixed-point count
 *   - unk_368 = that Animation's speed (+0x0c); Behavior copies mAnimSpeed
 *     into it, and InitResources sets mAnimSpeed to 0x1000, which is 1.0
 *
 * SIZE IS THE ROM'S OWN, not a rounded-up field span: `HootTheOwl_Spawn` calls
 * `fBase_c::operator new(1016)` -- 0x3f8 -- and stores `_ZTV10HootTheOwl`,
 * so that literal IS this class's sizeof. The observed fields only span to
 * 0x3f4; the difference is trailing space no source reads.
 *
 * SM64DS proves this class as daOwl_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daOwl_c_classInit at 0x02136798 (historical alias HootTheOwl_Spawn)
 * allocates 0x3f8 and installs this class's cartridge vtable. It backs the
 * OWL registry profile, whose descriptor at 0x02136a34 is reconstructed as
 * g_profile_OWL.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct HootTheOwl : dEnemyBase_c {
    /* What mCurrentState points at. The field was declared `s32` and every one
       of its six uses immediately cast it to a pointer, which is the whole
       evidence for the type: Behavior compares it against four objects in
       ov094's data and calls the handler at +0x08 through it.
       Only that handler is evidenced -- nothing reads the first eight bytes. */
    struct State {
        u8  pad_00[0x8];
        void (HootTheOwl::*mMain)();  /* 0x08 */
    };

    dCcAcPos_c mdCcAcPos_c; /* 0x110 */
    dBgCh_Actr mWithMeshClsn;       /* 0x150 */
    ModelAnim mModelAnim;             /* 0x30c */
    ShadowModel mShadowModel;         /* 0x370 */
    u8  pad_398[0x30];
    State *mCurrentState;             /* 0x3c8 */
    s32 unk_3cc;                      /* 0x3cc */
    u8  pad_3d0[0x4];
    u8  unk_3d4;                      /* 0x3d4 */
    u8  pad_3d5[0xf];
    u8  unk_3e4;                      /* 0x3e4 */
    u8  pad_3e5[0xb];
    Fix12i mAnimSpeed;                /* 0x3f0 -- copied into mModelAnim.speed */
    u8  pad_3f4[0x4];

    /* --- vtable --- */
    virtual ~HootTheOwl();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char HootTheOwl_size_must_be_0x3f8[sizeof(HootTheOwl) == 0x3f8 ? 1 : -1];

#endif /* HOOTTHEOWL_H */
