#ifndef DAOTS_C_H
#define DAOTS_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

/* The shared base of the three Bully variants. The ROM's RTTI names it daOts_c and
 * gives it exactly three children: daDonketu_c, daBDonketu_c and
 * daIDonketu_c. All three ROM names are the tree's names too now; the coined
 * `ChillBully` this file used to gloss daIDonketu_c with is gone. The production name
 * daOts_c is kept, as #1400 did for
 * dScMgBase_c -- it also makes the compiler's _ZTV7daOts_c agree with the RTTI
 * string for free.
 *
 * FOUR WITNESSES, which is one more than a leaf class ever gets:
 *
 *   _ZN7daOts_cD1Ev (ov064 0x02115ee0, 0x48) -- this class's own destructor. It stores
 *   data_ov064_0211b768 and destroys dExtShadowModel_c 0x370, dCcAc_c 0x33c,
 *   dBgCh_Actr 0x174, ModelAnim 0x110, then chains to _ZN12dEnemyBase_cD2Ev. D0 at
 *   0x02115f28 is the same plus Memory::Deallocate.
 *
 *   An out-of-line D1/D0 pair and the inline destructor below are not in conflict:
 *   mwcc inlines a virtual destructor at every direct-call site AND emits one
 *   out-of-line copy for the vtable to point at. The children inline it (no `bl`
 *   anywhere), and slots 16/17 of _ZTV7daOts_c hold these two addresses.
 *
 *   daDonketu_c_classInit, daBDonketu_c_classInit and daIDonketu_c_classInit are the SAME function apart
 *   from the allocation size and the last instruction. Each stores TWO vtables --
 *   this class's, from the inlined base constructor, and then its own -- with the
 *   four members constructed between them at those same four offsets.
 *
 * The vtable is data_ov064_0211b768: the word before it is 0x021138bc, which is
 * _ZTI7daOts_c. (Reading that address out of the merged symbol table answers
 * `data_ov009_021138bc`, and the destructor answers `_ZN10daHolhei_cD1Ev` -- ov062
 * through ov066 all load their .text at 0x02115ee0. Resolve inside ov064 or the
 * table will lie to you.)
 *
 * SIZE 0x398, and every member closes exactly on the next: ModelAnim 0x64 ends at
 * 0x174, dBgCh_Actr 0x1bc ends at 0x330, mFileTable plus two particle ids end at
 * 0x33c, dCcAc_c 0x34 ends at 0x370, dExtShadowModel_c 0x28 ends at 0x398. Nothing
 * allocates a plain daOts_c, so the ceiling comes from the children instead: the
 * earliest own named field any of the three declares is at 0x3fa.
 *
 * 0x398..0x3f9 ARE WRITTEN by this class's helpers (state, pos snapshot, shadow
 * matrix, counters). They still stay the children's padding: annexing them as
 * ordinary members would end this class at 0x3fa, which rounds up to 0x3fc and
 * moves the fields daBDonketu_c and daIDonketu_c declare at 0x3fa and after
 * (mSecretSoundCounter, mStarIdx); daDonketu_c's mBigBullyID at 0x3fc would stay
 * put, but pad_398 would have to shrink on all three. So the bytes are described
 * by the nested Tail struct below (a view of the whole object, padded out to
 * 0x398) and reached through tail(),
 * whose size is checked by the assertions at the bottom of the file.
 */
/* THE VTABLE, all 37 slots diffed against dEnemyBase_c's and against all three children's:
 *
 *   slot 0  InitResources    LITERAL ZERO here -- pure virtual. All three children
 *   slot 6  Behavior         override both, which is why they must.
 *   slot 3  CleanupResources 0x02116ca0 in ALL FOUR tables, so it is this class's.
 *   slot 9  Render           0x02116cf0 here, in daDonketu_c and in daIDonketu_c; daBDonketu_c
 *                            is the only one that overrides it.
 *   slot 29 OnAimedAtWithEgg 0x02115f84 in all four, so also this class's.
 *   slot 16/17               the destructor pair, one per class.
 *
 * THE NAMING DEFECT THIS NOTE USED TO RECORD IS NOW FIXED. The tree called 0x02116ca0
 * and 0x02116cf0 `_ZN11daDonketu_c16CleanupResourcesEv` and `_ZN11daDonketu_c6RenderEv`; the table
 * above says they are daOts_c's, because daDonketu_c does not override either -- it inherits
 * them. Both are renamed, and so are the three placeholders: 0x02115f84 (which carried
 * a `daBDonketu_c::OnAimedAtWithEgg -- recovered from vtable slot identity` comment,
 * naming a BASE's method after a CHILD) and the destructor pair.
 *
 * The blast radius turned out to be five files and five symbol lines: no source
 * anywhere referenced any of the five by name, which is what makes a misattributed
 * ROM symbol so easy to leave in place and so cheap to correct.
 *
 * The two pure-virtual slots ARE declared `= 0` below, and the note that used to say
 * they deliberately were not rested on a premise that has since stopped being true.
 * It argued this class's vtable is never emitted, so nothing could disagree with the
 * cartridge. That held while the inline destructor left the class without a key
 * function -- but `virtual int CleanupResources();` is declared non-inline here and
 * DEFINED in src/game/actors/d_a_ots.cpp, which makes it the key function
 * and makes that translation unit emit _ZTV7daOts_c. Measured: the emitted table
 * carried dEnemyBase_c's concrete InitResources and Behavior in slots 0 and 6 where
 * the cartridge has zeros, eight bytes wrong, and no byte gate could see it -- the
 * ROM build compares .text only. The zero words were always the evidence; the
 * declaration is what makes the compiler act on it.
 */
struct daOts_c : dEnemyBase_c {
    /* A view of the whole object that names the bytes 0x398..0x3f9, which the
       children still declare as pad_398 (see the SIZE note above). The offsets in
       the comments are object offsets: the struct starts at the top of the object
       and pads out to 0x398, so a field's address folds to this + its offset (a
       view that started at 0x398 costs an extra add wherever a field's address is
       taken). */
    struct Tail {
        u8  pad_000[0x398];
        s32 mState;             /* 0x398 -- State */
        /* Where the actor stood at the top of the last Behavior frame that ran
           (BehaviorCommon copies mPos here before moving it); the ledge check puts
           x and z back to it. InitResourcesCommon seeds it with the spawn position. */
        s32 mPosBeforeMoveX;    /* 0x39c */
        s32 mPosBeforeMoveY;    /* 0x3a0 */
        s32 mPosBeforeMoveZ;    /* 0x3a4 */
        /* InitResourcesCommon seeds this with the spawn position. STATE_RETURN_HOME
           steers toward it and the two player-range tests are measured from it (the
           chase-keeping one uses only its x and z, at the actor's own height). The death
           handler overwrites it with the spot the actor died at (y set 5 units above
           the floor), which is where func_ov064_0211616c spawns its particles. */
        s32 mHomePosX;          /* 0x3a8 */
        s32 mHomePosY;          /* 0x3ac */
        s32 mHomePosZ;          /* 0x3b0 */
        /* Handed to DropShadowRadHeight; func_ov064_02116bac rebuilds it every
           frame as a Y rotation by mAngleY with the translation row set to the
           position divided by 8. */
        Matrix4x3 mShadowMtx;   /* 0x3b4 */
        /* Horizontal angle from the actor to mHomePos (Vec3_HorzAngle), which
           STATE_RETURN_HOME turns toward. */
        s16 mHomeAngle;         /* 0x3e4 */
        u8  pad_3e6[0x2];
        /* Copied from the config block in InitResourcesCommon: horzDecel, modelYOffset,
           shadowRadius. mHorzDecel is the rate STATE_KNOCKED_BACK slows mHorzSpeed
           to zero. mModelYOffset is added to mPosY for the model matrix and for the
           floor probe's origin. mShadowRadius is the drop shadow's radius at ground
           level; func_ov064_02116bac shrinks it as the actor rises. */
        s32 mHorzDecel;         /* 0x3e8 */
        s32 mModelYOffset;      /* 0x3ec */
        s32 mShadowRadius;      /* 0x3f0 */
        /* Floor height under the actor, from func_ov064_02116220's probe. */
        s32 mFloorY;            /* 0x3f4 */
        u8  pad_3f8[0x1];
        /* STATE_KNOCKED_BACK: hops done so far (it stops hopping at 2); zeroed
           whenever a hit puts the actor in that state. */
        u8  mHopCount;          /* 0x3f9 */
    };

    /* The object viewed as a Tail. */
    Tail &tail() { return *(Tail *)this; }

    ModelAnim           mModelAnim;             /* 0x110 */
    dBgCh_Actr        mWithMeshClsn;          /* 0x174 */
    /* Pointer to a per-variant config block (this TU reads it through CONFIG()). All
       three children declare a field here, which is what makes it the base's
       rather than any one of theirs. daDonketu_c::InitResources points it at
       data_ov064_0211b834; daOts_c::CleanupResources (inherited by daDonketu_c)
       releases five SharedFilePtrs through it. daBDonketu_c's own header called it
       a u8; two of the three call it a word, and the use above is a pointer, so
       a word it is. */
    s32                 mFileTable;             /* 0x330 */
    /* Particle::System::New unique ids. func_ov064_0211616c stores both; the
       eight bytes were pad_334 while untyped. */
    u32                 mParticle0;             /* 0x334 */
    u32                 mParticle1;             /* 0x338 */
    dCcAc_c  mdCcAc_c;    /* 0x33c */
    dExtShadowModel_c         mShadowModel;           /* 0x370 */

    /* INLINE, AND IT MUST BE. Both children's destructors store their own vtable,
       then this one's, then destroy all four members in place -- there is no `bl`
       to a base destructor anywhere. That is what an inline destructor compiles to,
       and an out-of-line declaration here would make each child emit a call the ROM
       does not have. Being inline also leaves this class without a key function, so
       merely including this header does not emit _ZTV7daOts_c. Same reasoning, same
       wording, as include/dBgActor_c.h. */
    virtual ~daOts_c() {}

    /* The three slots this class owns outright, each named by the diff above rather
       than by any one child's source. InitResources and Behavior are the pure-virtual
       pair, declared at the end of the class. */
    virtual int CleanupResources();     /* slot  3 */
    virtual int Render();               /* slot  9 */
    virtual int OnAimedAtWithEgg();     /* slot 29 */
    /* New slots 31..36. Their positions and override ownership are fixed by the
       four ROM vtables. The spellings describe their call sites; the stripped
       image carries no original method names. */
    virtual int  UpdateRunState();      /* slot 31 -- called only from state 1 */
    virtual void UpdateDeathState();    /* slot 32 -- called only from state 4 */
    virtual void PlayStepSound();       /* slot 33 -- animation frames 4 and 7 */
    virtual void PlayHitSound();        /* slot 34 -- entering the hit state */
    virtual void PlayShellHitSound();   /* slot 35 -- shell/knockback response */
    virtual void PlayDeathSound();      /* slot 36 -- entering state 4 */

    /* THE NULL SLOTS THE NOTE ABOVE ALREADY NAMES, SPELT SO THE COMPILER AGREES.
       mwccarm lays down a bare 0x00000000 with no relocation for a pure virtual --
       there is no __cxa_pure_virtual in this image for it to point at -- so a zero
       word in a ROM vtable IS the `= 0`, and it is the only thing that produces one.
       Left undeclared, this class silently inherits dEnemyBase_c's concrete bodies and
       the vtable it emits disagrees with the cartridge at exactly these slots.
       Measured by tools/romdata_check.py, which is the only gate that reads them:
       the ROM build's 106/106 compares .text alone and is blind here.

       DECLARED LAST, AND WITH `virtual` -- unlike the plain overrides above. The
       pure-specifier is only valid on a declaration carrying the keyword, and a
       pure virtual has no body to emit, so it can never become the key function:
       whichever virtual was first and non-inline before is still first now. */
    virtual int InitResources() = 0;        /* slot  0 */
    virtual int Behavior() = 0;             /* slot  6 */

    /* Non-virtual shared workers. All three children wrap these from the
       pure-virtual InitResources / Behavior slots after their own preamble.
       Class ownership is proven by this-pointer layout (ModelAnim 0x110,
       dBgCh_Actr 0x174, mFileTable 0x330, dCcAc_c 0x33c, dExtShadowModel_c 0x370)
       and by named daOts callees already in this TU. The English spellings
       describe those call sites; the stripped image carries no original
       method names. */
    int InitResourcesCommon();
    int BehaviorCommon();
    void func_ov064_02115f98(char* a1);
    int func_ov064_02116110(short step);
    int func_ov064_0211616c();
    void func_ov064_02116220();
    void func_ov064_02116460();
    int func_ov064_02116560();
    void func_ov064_021165d8();
    int func_ov064_021166f0();
    void func_ov064_02116754();
    void func_ov064_02116bac();

    /* Tail::mState (reached through tail()), written by BehaviorCommon's switch and by the helpers it calls.
       Which child classes use which states is theirs to say; these names describe
       what the shared handlers in src/game/actors/d_a_ots.cpp do. */
    enum State {
        STATE_RETURN_HOME = 0,  /* walks toward mHomePos at 5 units a frame; switches to STATE_CHASE once the player is within 800 units of it */
        STATE_CHASE = 1,        /* the child's UpdateRunState runs; back to STATE_RETURN_HOME when the player is not within 1000 units of mHomePos */
        STATE_KNOCKED_BACK = 2, /* after a hit: slides to a stop, hops twice, waits, then resumes STATE_CHASE */
        STATE_LEDGE_TURN = 3,   /* IsGoingOffCliff fired: turns about, then back to STATE_RETURN_HOME after 15 frames */
        STATE_DYING = 4,        /* floor surface type 1 under it: death animation; UpdateDeathState (the child's) runs */
        STATE_REMOVE = 5        /* floor surface type 4, 5 or 0x13 under it: MarkForDestruction */
    };
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daOts_c_size_must_be_0x398[sizeof(daOts_c) == 0x398 ? 1 : -1];
/* Tail is a view over the whole object, ending with the last byte this class's
   helpers write (0x3f9); 0x3fa bytes round up to 0x3fc. It is NOT a member
   layout: the children's own fields start at 0x3fa. */
typedef daOts_c::Tail daOts_c_Tail;
typedef char daOts_c_Tail_size_must_be_0x3fc[sizeof(daOts_c_Tail) == 0x3fc ? 1 : -1];
#endif

#endif /* DAOTS_C_H */
