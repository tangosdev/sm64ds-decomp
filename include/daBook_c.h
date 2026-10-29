#ifndef DABOOK_C_H
#define DABOOK_C_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout: the
 * class's own destructor `_ZN8daBook_cD1Ev` destroys each member, and
 * `daBook_c_classInit_SHOOT_BOOK` constructs the same types at the same offsets after
 * storing `_ZTV8daBook_c`. Everything this header used to restate below 0x110
 * belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a reading
 * rather than a guess:
 *
 *     0x110 dExtAnmModel_c                  0x64    -> 0x174
 *     0x174 dExtSimpleModel_c                      0x50    -> 0x1c4
 *     0x1c4 dExtShadowModel_c                0x28    -> 0x1ec
 *     0x1ec Matrix4x3                  0x30    -> 0x21c
 *     0x21c dCcAcPos_c  0x40    -> 0x25c
 *     0x25c dBgCh_Actr               0x1bc   -> 0x418
 *
 * SIZE IS THE ROM'S OWN: `daBook_c_classInit_SHOOT_BOOK` calls `fBase_c::operator new(1108)`
 * -- 0x454 -- and stores this class's vtable, so that literal IS this
 * class's sizeof.
 *
 * NAME. The cartridge's RTTI names this class daBook_c: _ZTS8daBook_c at
 * ov020 0x0211482c, _ZTI8daBook_c at 0x02114844, _ZTV8daBook_c at 0x0211495c.
 * It was carried here under the coined name BookShot until that was replaced
 * by the ROM's. The reconstructed factory
 * daBook_c_classInit_SHOOT_BOOK (historical alias BookShot_Spawn) installs this class's
 * cartridge vtable for the SHOOT_BOOK registry profile.
 */

#include "dEnemyBase_c.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

struct daBook_c : dEnemyBase_c {
    dExtAnmModel_c mAnmModel;                                 /* 0x110 */
    dExtSimpleModel_c mModel;                                 /* 0x174 */
    dExtShadowModel_c                  mShadowModel;          /* 0x1c4 */
    /* Drop-shadow matrix: InitResources copies IDENTITY_MATRIX4X3 here and
       func_ov020_0211216c writes the position, divided by 8, into its
       translation row before handing it to DropShadowRadHeight. */
    Matrix4x3                    mShadowMat;            /* 0x1ec */
    dCcAcPos_c    mdCcAcPos_c; /* 0x21c */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x25c */
    /* Unique ID of the actor this book works with: every read hands it to
       dActor_c::FindWithID. Nothing in this file writes it after the zero
       InitResources stores, so the owner is another actor. */
    u32                          mLinkedActorID;        /* 0x418 */
    /* The player that touched or hit this book. func_ov020_021115ac stores it;
       func_ov020_02111340 and the player-damage paths read it back. */
    Player                      *mTouchedPlayer;        /* 0x41c */
    s32                          mKind;                 /* 0x420 -- Kind */
    s32                          mState;                /* 0x424 -- State */
    /* mState as it was when Behavior diverted into STATE_YOSHI_SKID. */
    s32                          mSavedState;           /* 0x428 */
    /* Where InitResources found the book; BOOK_SWITCH slides on z relative
       to mHomePosZ, and the drop shadow stays at mHomePosY. */
    s32                          mHomePosX;             /* 0x42c */
    s32                          mHomePosY;             /* 0x430 */
    s32                          mHomePosZ;             /* 0x434 */
    /* Cylinder offset handed to dCcAcPos_c::Init and SetPosRelativeToActor. */
    Vector3                      mClsnOffset;           /* 0x438 */
    /* Direction from the book to the closest player, as of the last
       func_ov020_021112b0. Pitch and yaw are atan2 results; roll is the
       constant 0x4000 the wind-up turns toward. */
    s16                          mAimPitch;             /* 0x444 */
    s16                          mAimYaw;               /* 0x446 */
    s16                          mAimRoll;              /* 0x448 */
    u8  pad_44a[0x2];
    /* Uniform scale in fix12 (0x1000 is 1.0): 0.5 at spawn, then grows to 1.5
       through the wind-up. It is copied into mScaleX/Y/Z and sizes the collider. */
    s32                          mUniformScale;         /* 0x44c */
    /* Nonzero once the animated model is in use: Render draws mAnmModel
       instead of mModel, and func_ov020_0211216c builds the matrix in it. */
    u8 mUsesAnmModel; /* 0x450 */
    u8  pad_451[0x3];

    /* --- vtable --- */
    virtual ~daBook_c();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    /* --- non-virtual --- */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* State handlers and helpers, in ROM order. They keep their ROM-address
       names: no EAD name survives for any of them. */
    void func_ov020_021112b0();   /* aims the book at the closest player */
    void func_ov020_02111340();   /* wrong push: fires a SHOOT_BOOK beside the shelf */
    int  func_ov020_02111418();   /* acts on the func_ov020_021115ac result; 1 = book gone */
    int  func_ov020_021115ac();   /* reads the collider, returns a HIT_ result */
    void func_ov020_0211174c();   /* KIND_SWITCH_BOOK state machine */
    void func_ov020_021119dc();   /* STATE_YOSHI_SKID */
    void func_ov020_02111aa8();   /* STATE_SPAWNED */
    void func_ov020_02111b28();   /* STATE_FLY */
    void func_ov020_02111c30();   /* STATE_WIND_UP */
    void func_ov020_02111ee0();   /* STATE_TILT_BACK */
    void func_ov020_02111fc4();   /* STATE_WAIT */
    void func_ov020_02112080();   /* KIND_FLYING_BOOK state machine */
    void func_ov020_02112110();   /* drops the coin, puffs, removes the actor */
    void func_ov020_0211216c();   /* rebuilds the model matrix and the drop shadow */

    /* Which of the two behaviours the profile selected; InitResources sets it
       from actorID and Behavior switches on it. */
    enum Kind {
        KIND_FLYING_BOOK = 0,   /* SHOOT_BOOK and KILLER_BOOK: run mState 0..5 */
        KIND_SWITCH_BOOK = 1    /* BOOK_SWITCH: runs mState 6..10 */
    };

    /* mState, for both kinds. */
    enum State {
        /* KIND_FLYING_BOOK */
        STATE_WAIT = 0,         /* KILLER_BOOK idles until the player is close and in front */
        STATE_TILT_BACK = 1,    /* tips back, then swaps the plain model for the animated one */
        STATE_WIND_UP = 2,      /* plays its animation, turns to face the player and swells, then launches */
        STATE_FLY = 3,          /* travels in a straight line until it hits something */
        STATE_SPAWNED = 4,      /* SHOOT_BOOK starts here: waits three frames, then jumps to full speed and STATE_FLY */
        STATE_YOSHI_SKID = 5,   /* slides to a stop after Yoshi lets go, then resumes mSavedState */
        /* KIND_SWITCH_BOOK */
        STATE_SWITCH_WAIT = 6,  /* waits for the daTrsTrap_c bookshelf to raise bit 3 of its mBookFlags */
        STATE_SWITCH_RETRACT = 7, /* slides back into the shelf */
        STATE_SWITCH_READY = 8, /* collider on; waits to be hit */
        STATE_SWITCH_PUSH = 9,  /* slides forward, then reports itself to the partner */
        STATE_SWITCH_DONE = 10  /* waits for the partner's verdict */
    };
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daBook_c_size_must_be_0x454[sizeof(daBook_c) == 0x454 ? 1 : -1];
#endif

#endif /* DABOOK_C_H */
