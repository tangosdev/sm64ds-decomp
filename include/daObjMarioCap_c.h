#ifndef DAOBJMARIOCAP_C_H
#define DAOBJMARIOCAP_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daObjMarioCap_c -- daObjMarioCap_c_classInit (was Cap_Spawn), g_profile_OBJ_MARIO_CAP (was Cap_SpawnInfo)
 */

#include "types.h"
#include "dEnemyBase_c.h"
#include "CapIcon.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Derives from dEnemyBase_c, and both witnesses agree offset for offset:
 *
 *   daObjMarioCap_c_classInit (ov002) allocates 0x410, calls _ZN12dEnemyBase_cC2Ev, stores
 *   _ZTV15daObjMarioCap_c, then constructs dCcAc_c 0x110, dBgCh_Actr 0x144,
 *   ModelAnim 0x300, dExtShadowModel_c 0x364 and the CapIcon at 0x3d0.
 *
 *   _ZN15daObjMarioCap_cD1Ev tears the same five down in exactly the reverse order and
 *   chains to _ZN12dEnemyBase_cD2Ev.
 *
 * THE 0x3d0 MEMBER IS dCapIcon_c (the CapIcon compatibility spelling), whose
 * ROM RTTI and two-slot vtable identify its constructor/destructor at
 * 0x020ab3c4 / 0x020ab3a0. It is the same member dCapEnemy_c holds at 0x164.
 * Left as padding the destructor emits a short chain and comes out a different
 * SIZE, which reads as `999 word(s) differ` and looks like a total failure
 * rather than one missing member.
 *
 * SIZE 0x410, the literal in daObjMarioCap_c_classInit's fBase_c::operator new. CapIcon is 0x1c, so
 * 0x3d0 + 0x1c = 0x3ec closes onto the scalars below it.
 *
 * This class used to be named WaterfallMist; RTTI ov002:0x021095ac names
 * 15daObjMarioCap_c at vtable 0x021095f0, and the waterfall name belongs to
 * daObjWaterfall_c.
 */
struct daObjMarioCap_c : dEnemyBase_c {
    dCcAc_c  mdCcAc_c;    /* 0x110 */
    dBgCh_Actr        mWithMeshClsn;          /* 0x144 */
    ModelAnim           mModelAnim;             /* 0x300 */
    dExtShadowModel_c         mShadowModel;           /* 0x364 */
    /* Drop-shadow matrix: UpdateMatrix rebuilds it on the frames the shadow is drawn (rotation
       about Y, translation = position / 8) for DropShadowRadHeight. */
    Matrix4x3           mShadowMat;             /* 0x38c */
    /* The current state: a pointer to one of nine {enter, per-frame} records
       in ov002 .bss (data_ov002_0210df04 .. df84), each a pair of 8-byte
       pointers-to-member filled in at static-init time by
       __sinit_ov002_02101064. EnterState
       stores a record here and runs its first member (the enter function);
       Behavior then runs the second one every frame. Held as an s32 because
       the member-pointer record layout is spelled by the file-local
       CapStateRec / CapEnterSelf stand-ins in the .cpp, not by this header. */
    s32 mStateEntry;                            /* 0x3bc */
    /* The player this cap is bound to: whoever touched it, or the nearest
       player (ClosestPlayer) for the types that start bound. Read as a
       Player*; the touch check stores FindWithID's result here before it
       tests the actor ID, so it can briefly hold a non-Player actor after a
       failed touch. */
    Player *mPlayer;                            /* 0x3c0 */
    /* Where InitResources found the cap; the taken state respawns a
       TYPE_RESPAWNING cap here. */
    s32 mHomePosX;                              /* 0x3c4 */
    s32 mHomePosY;                              /* 0x3c8 */
    s32 mHomePosZ;                              /* 0x3cc */
    dCapIcon_c mCapIcon;                        /* 0x3d0 */
    /* Blink: bit 0 set means "skip drawing this frame" (Render ignores it in
       the taken state). The sliding state writes it from a low bit of
       mStateTimer once the timer is under half of mStartTimer (bit 2, then
       bit 1 once it is under a quarter, so the blink speeds up); the enter
       functions of the taken, animation and idle states zero it. */
    s32 mBlinkHidden;                           /* 0x3ec */
    /* param1 & 0xff, with 0xff turned into 0: selects the behaviour; see Type. */
    s32 mType;                                  /* 0x3f0 */
    /* Which character's cap this is, 0..2 (param1 bits 8..11; 3 and up make
       InitResources fail). Indexes the model table data_ov002_020ff0ac and the
       per-character cap-icon list, and is what the pickup hands to
       Player::SetNewHatCharacter. */
    s32 mModelIndex;                            /* 0x3f4 */
    u8  pad_3f8[0x4];
    /* Direction the floor under the cap slopes toward: atan2 of the floor
       normal's x and z, refreshed each frame by the sliding state, for types
       5, 7, 9 and 11 only, while it is on the ground. 0x10000 is a full turn. */
    s16 mSlopeAngle;                            /* 0x3fc */
    /* Set once the cap's model has been given its follow-up animation: the
       0x8012 animation in the taken state (OnTurnIntoEgg sets it too) or the
       second animation of type 0x14 in the animation state, whose enter
       function (InitAnim) clears it. */
    u8  mAnimStarted;                           /* 0x3fe */
    /* 1 while bit 1 of mCapIcon.mFlags is clear (the bit dCapIcon_c's
       GetCapState tests); only caps with an icon ever set it. Behavior then
       does nothing and Render draws nothing. When the bit comes on, the cap
       pops in (see mPopIn). */
    u8  mDormant;                               /* 0x3ff */
    /* Cap-icon kind. param1 bits 12..15, clamped to 0..2 (types 4 and 17) or
       0..1, then overridden by most types' cases in InitResources (types 4
       and 17 keep the param1 value). 0xff means the cap has no icon:
       Behavior and OnPendingDestroy skip the icon work, and since mDormant is
       only set for caps with an icon, such a cap is never dormant. Otherwise it is handed to dCapIcon_c::func_ov001_020ab228,
       which keeps it in the icon's unk_19. */
    u8  mIconKind;                              /* 0x400 */
    /* Step of the taken state: 1 = waiting for Player::SetNoControlState(0xf)
       to succeed (set for TYPE_START_TAKEN and when the touch finds the player
       has lost the cap), 2 = the hat has been handed over, waiting for the
       animation to end. 0 for an ordinary pickup. */
    u8  mTakenStep;                             /* 0x401 */
    /* 1 while the cap is scaling in after its icon bit came on: mScaleX
       starts at 0 and chases mPopScale, which itself settles to 1.0. Cleared
       once the cap is on the ground at full scale. */
    u8  mPopIn;                                 /* 0x402 */
    /* Written 1 on entering the taken state; nothing in this file reads it. */
    u8  unk_403;                                /* 0x403 */
    /* Copy of the timer the sliding state's enter function loads into
       mStateTimer (types 5/7/9: 210 frames in game mode 1, otherwise 120;
       type 18: 300; type 11: 180). The blink compares against it. */
    u16 mStartTimer;                            /* 0x404 */
    u8  pad_406[0x2];
    /* mHorzSpeed as saved by the sliding state each frame the cap is airborne;
       nothing in this file reads it back. */
    s32 unk_408;                                /* 0x408 */
    /* Pop-in scale target, fix12: starts at 0x2000 (2.0), eased toward 0x1000
       (1.0) at 0x200 a frame while mScaleX chases it at 0x400 a frame. */
    s32 mPopScale;                              /* 0x40c */

    /* INLINE, AND DECLARED FIRST. The cartridge puts D1 at 0x020b6f18 below
       D0 at 0x020b6f68 and carries no D2, which is exactly what mwccarm 2004
       emits for an inline destructor; an out-of-line one emits D2/D0/D1 in the
       wrong order plus a homeless D2. The typed member list below makes the
       empty body own the dCapIcon_c, dExtShadowModel_c, ModelAnim, dBgCh_Actr and
       dCcAc_c teardowns and the chain into _ZN12dEnemyBase_cD2Ev.

       With the destructor inline, OnYoshiTryEat becomes the first out-of-line
       virtual this class declares -- the key function -- so the vtable and the
       RTTI group land in the translation unit that defines it,
       src/actors/daObjMarioCap_c.cpp. */
    virtual ~daObjMarioCap_c() {}

    virtual s32   OnYoshiTryEat();         /* slot 18 -- key function */

    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void OnPendingDestroy();
    void OnTurnIntoEgg(Player &player);  /* slot 19, ov002 0x020b81e0 */

    /* Once-a-frame render-matrix update and drop-shadow placement. */
    void UpdateMatrix();
    /* The state dispatcher: stores a {enter, per-frame} record in mStateEntry
       and calls its enter member. `void*` because the record's member-pointer
       layout is spelled by the .cpp's file-local stand-ins. */
    int EnterState(void *rec);
    /* The pickup check shared by the states that let the player take the cap:
       reflects off walls, validates the collider owner is a Player, hands the
       hat over and enters the taken state. */
    void CheckTouch();

    /* The nine {enter, per-frame} state pairs, in the state-table's order. */
    int InitWait();         /* df64 enter;  type 0 */
    int Wait();             /* df64 per-frame */
    int InitTimedWait();    /* df84 enter;  type 1 */
    int TimedWait();        /* df84 per-frame */
    int InitTouchWait();    /* df04 enter;  type 2 */
    int TouchWait();        /* df04 per-frame */
    int InitTimedWait2();   /* df24 enter;  type 3 */
    int TimedWait2();       /* df24 per-frame */
    int InitSlide();        /* df34 enter;  types 4..9, 11, 16..18 */
    int Slide();            /* df34 per-frame */
    int InitTaken();        /* df54 enter;  the taken state */
    int Taken();            /* df54 per-frame */
    int InitAnim();         /* df74 enter;  types 10, 15, 20..22 */
    int Anim();             /* df74 per-frame */
    int InitFall();         /* df14 enter;  type 12 */
    int Fall();             /* df14 per-frame */
    int InitDormant();      /* df44 enter;  type 13 */
    int Dormant();          /* df44 per-frame */

    /* Leaf until fBase_c can declare operator new (#2570). unsigned long, not
       unsigned int: size_t is unsigned int on this include path and mangles
       nwEj, colliding with fBase_c's own allocator. */
    static void *operator new(size_t size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    /* What the cap is, from the low byte of param1 (0xff reads as 0). Only the
       values the code gives a meaning to are named; the rest are plain numbers
       in InitResources' switch, where most pick a state (see mStateEntry), a
       collider size and a cap-icon kind (types 23..0xfe enter no state).
       Bits 8..11 of param1 are the cap's character (mModelIndex), bits 12..15
       the icon kind (mIconKind, range-checked there). */
    enum Type {
        TYPE_RESPAWNING = 4,     /* (param1 type 14 is rewritten to this by InitResources.)
                                    Once the player has finished taking it, the taken-state
                                    handler spawns a fresh OBJ_MARIO_CAP with the current
                                    param1 at mHomePos */
        TYPE_VANISH_LUIGI_A = 6, /* touch calls Player::InitVanishLuigi and removes the cap;
                                    just runs the touch check */
        TYPE_VANISH_LUIGI_B = 7, /* the touch acts like 6, but this one has a 210 / 120 frame
                                    countdown (game mode 1 / others), slides, blinks and
                                    expires */
        TYPE_METAL_WARIO_A = 8,  /* touch calls Player::InitMetalWario and removes the cap;
                                    just runs the touch check */
        TYPE_METAL_WARIO_B = 9,  /* the touch acts like 8, with the same countdown, slide
                                    and blink as 7 */
        TYPE_START_TAKEN = 19    /* InitResources puts it straight into the taken state */
    };
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjMarioCap_c_size_must_be_0x410[sizeof(daObjMarioCap_c) == 0x410 ? 1 : -1];
#endif

#endif /* DAOBJMARIOCAP_C_H */
