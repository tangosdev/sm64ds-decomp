#ifndef DAPOPOI_C_H
#define DAPOPOI_C_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN9daPopoi_cD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 dCcAc_c         0x34   -> 0x144
 *     0x144 dCcAcPos_c  0x40   -> 0x184
 *     0x184 dBgCh_Actr               0x1bc  -> 0x340
 *     0x340 ModelAnim                  0x64   -> 0x3a4
 *     0x3a4 dExtShadowModel_c                0x28   -> 0x3cc
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x390 mAnimation   = mModelAnim + 0x50
 *   - 0x39c unk_39c      = mModelAnim + 0x5c
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE ROM'S OWN, not a rounded-up field span: `daPopoi_c_classInit` calls
 * `fBase_c::operator new(1068)` -- 0x42c -- and stores `_ZTV9daPopoi_c`,
 * so that literal IS this class's sizeof. The last field, mSoundHandle at
 * 0x428, ends exactly there.
 *
 * SM64DS RTTI names the implementation daPopoi_c. The reconstructed
 * factory daPopoi_c_classInit (historical alias
 * HeaveHo_Spawn) constructs it for the POPOI
 * registry profile.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"

/* Actor IDs this class compares against (symbols/actor_debug_names.tsv). */
enum {
    daPopoi_ACTOR_PLAYER = 191      /* PLAYER */
};

/* Sound IDs, both bank 3. Neither is a name recovered from the cartridge; they say
 * when the sound plays. */
enum {
    daPopoi_SND_MOVE_LOOP    = 0x186,   /* PlayLong (with its argument 3), kept alive through mSoundHandle
                                           by the Wander and Chase update handlers */
    daPopoi_SND_THROW_PLAYER = 0x10b    /* played at the actor's position right after the launch call succeeds */
};

/* The state machine is a pointer (mState) to one of five 16-byte records built at
 * startup by __sinit_ov077_021275fc into data_ov077_02127cd8..02127d18. Each record
 * is two pointer-to-member words: an ENTER handler (run once by func_ov077_02126d5c as
 * the state is installed) then an UPDATE handler (run each frame by Behavior). The
 * names below describe what each pair does; they are not labels from the cartridge.
 *
 *   record (data_ov077_...)   enter / update               what it does
 *   02127ce8  Wander          02126cd4 / 02126ad0          random heading, walks; home pull, chase trigger
 *   02127cf8  Pause           02126a84 / 02126a50          stands still 70 frames, then back to Wander
 *   02127d08  Chase           02126930 / 0212679c          turns toward the Player; gives up to Pause
 *   02127d18  TurnAway        02126a04 / 021269a8          turns a quarter turn after the ahead probe fires (only in level 0x2a)
 *   02127cd8  Grab            02126758 / 02126640          plays its animation; launches the Player; then Pause
 */
struct daPopoi_StateRecord;

struct daPopoi_c : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dCcAcPos_c    mdCcAcPos_c; /* 0x144 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x184 */
    ModelAnim                    mModelAnim;            /* 0x340 */
    dExtShadowModel_c                  mShadowModel;          /* 0x3a4 */
    u8  pad_3cc[0x30];                                   /* 0x3cc -- no function here touches it */
    /* The current state's record; see the table above. Installed by
       func_ov077_02126d5c, which also runs the record's enter handler. */
    struct daPopoi_StateRecord  *mState;                /* 0x3fc */
    /* The Player the sensor was touching when func_ov077_02126528 entered the Grab
       state. Zero from InitResources; cleared by func_ov077_02126640 once its launch
       call returns nonzero. If Grab ends (animation Finished) without a launch it is
       left set. */
    Player                      *mCaughtPlayer;         /* 0x400 */
    /* InitResources copies the actor's own position here once; nothing in this
       file writes it again. Behavior snaps the actor back to it when it falls
       below the water height, and the Wander/Chase handlers measure their
       distances from it. */
    s32                          mHomePosX;             /* 0x404 */
    s32                          mHomePosY;             /* 0x408 */
    s32                          mHomePosZ;             /* 0x40c */
    /* The actor's position at the end of the previous Behavior call. Behavior
       restores it when this frame's step would walk off a ledge or onto steep
       ground; the Wander/Chase handlers and the ahead probe restore it when
       the actor is against a wall (or the probe fires). */
    s32                          mSavedPosX;            /* 0x410 */
    s32                          mSavedPosY;            /* 0x414 */
    s32                          mSavedPosZ;            /* 0x418 */
    s32                          unk_41c;               /* 0x41c -- only ever zeroed (Wander and Chase enter) */
    /* The heading mPrevAngleY is stepped toward (ApproachLinear); Behavior then
       copies mPrevAngleY into mAngleY. Wander enter picks a random multiple of
       0x1000, Chase update sets the heading to the Player, TurnAway enter sets
       mAngleY + 0x4000. */
    s16                          mTargetAngleY;         /* 0x420 */
    /* Chase's per-frame angle step toward mTargetAngleY; Chase enter zeroes it
       and Chase update raises it toward 0x600 by 0x100 a frame. */
    s16                          mTurnRate;             /* 0x422 */
    u8  pad_424[0x2];
    /* A countdown (DecIfAbove0_Short in Behavior). TurnAway sets it to 30 frames
       when it finishes; while it is nonzero the Wander update skips the test for
       a Player to chase. */
    u16                          mCooldown;             /* 0x426 */
    /* Sound::PlayLong's return, fed back on the next call (daPopoi_SND_MOVE_LOOP). */
    u32                          mSoundHandle;          /* 0x428 */

    /* --- vtable --- */
    /* Inline and first: measured (class-form skill) that mwccarm 2004/b56
       emits the retail D1-then-D0 pair in ROM order plus _ZTV/_ZTI/_ZTS
       homed in the instantiating TU, and no leaf D2. Out-of-line emits
       D0 before D1, which the production isolate refuses. */
    virtual ~daPopoi_c() {}

    int Behavior();
    int InitResources();
    int Render();
    int CleanupResources();
    void OnPendingDestroy();

    /* Probe ahead for a wall/missing floor (Behavior and the Wander/Chase
       updates); 1 = blocked, with the position rolled back. Only casts rays in
       level 0x2a. */
    int  func_ov077_02126300();
    /* Sensor scan for a Player to grab; installs the Grab state. Behavior only. */
    void func_ov077_02126528();
    /* State installer: stores the record in mState and runs its enter handler. */
    int  func_ov077_02126d5c(daPopoi_StateRecord *next);
    /* Rebuilds mModelAnim.mat4x3 from mAngleY and mPos. Behavior only. */
    void func_ov077_02126dac();

    /* The five state records' handler pairs; see the table above. */
    int  func_ov077_02126758();   /* Grab    enter  (record 02127cd8) */
    int  func_ov077_02126640();   /* Grab    update */
    int  func_ov077_02126cd4();   /* Wander  enter  (record 02127ce8) */
    int  func_ov077_02126ad0();   /* Wander  update */
    int  func_ov077_02126a84();   /* Pause   enter  (record 02127cf8) */
    int  func_ov077_02126a50();   /* Pause   update */
    int  func_ov077_02126930();   /* Chase   enter  (record 02127d08) */
    int  func_ov077_0212679c();   /* Chase   update */
    int  func_ov077_02126a04();   /* TurnAway enter (record 02127d18) */
    int  func_ov077_021269a8();   /* TurnAway update */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daPopoi_c_size_must_be_0x42c[sizeof(daPopoi_c) == 0x42c ? 1 : -1];
#endif

#endif /* DAPOPOI_C_H */
