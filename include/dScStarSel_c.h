#ifndef DSCSTARSEL_C_H
#define DSCSTARSEL_C_H

#include "dScene_c.h"
#include "dExtSimpleModel_c.h"

/* One of dScene_c's ten direct children (see the census in dScene_c.h) --
 * the star-select screen shown before entering a course.
 *
 * BASE. dScene_c, confirmed by the RTTI edge (dScStarSel_c ov003:0x020b16bc
 * -> dScene_c arm9:0x020914d4, kind "si") and independently by the vtable:
 * _ZTV12dScStarSel_c (0x020b1704, ov003) is 18 slots, matching dScene_c's
 * own 18, and dScStarSel_c overrides exactly the same seven dScene_c gives
 * every other direct child -- 0, 3, 6, 9, 12, 16, 17 (rtti_vtables.py --own
 * dScStarSel_c). No new virtual is added.
 *
 * CONSTRUCTION. dScStarSel_c_classInit is the factory: it opens with
 * `_ZN7fBase_cnwEm(0x13c)`, i.e. fBase_c::operator new(0x13c). SIZE 0x13c
 * is read directly off the allocator call. The same factory then calls the
 * shared "construct N elements of size S" helper, __cxa_vec_ctor(p+0x64, 2,
 * 0x50, dExtSimpleModel_c::dExtSimpleModel_c, dExtSimpleModel_c::~dExtSimpleModel_c) -- two dExtSimpleModel_c objects (include/dExtSimpleModel_c.h,
 * itself confirmed sizeof(dExtSimpleModel_c) == 0x50) back to back at 0x064..0x104.
 * The matching destructor (func_ov003_020addfc / _020ade54) tears the same
 * range down with __cxa_vec_cleanup(p+0x64, 2, 0x50, dExtSimpleModel_c::~dExtSimpleModel_c) before
 * calling fBase_c's D2 -- two independent witnesses agreeing on both count
 * and stride. InitResources (func_ov003_020af8a0) confirms the same two
 * offsets from the other side: it calls dExtSimpleModel_c::LoadAndSetFile at arg0+0x64
 * and arg0+0xB4 (0x64 + 0x50), i.e. models[0] and models[1].
 *
 * MEMBERS BELOW 0x50 are all inherited (dScene_c/dBase_c add none of their
 * own -- see dScene_c.h). The scalars on either side of the dExtSimpleModel_c
 * pair are named from their uses in d_s_star_sel_methods.cpp; the
 * "Runtime" notes record blinded melonDS pokes (EU ROM, tier I, every sham
 * and control unchanged). The bytes nothing reads stay pad_ arrays.
 * Fixed-point values are 20.12 (0x1000 = 1.0).
 *
 * VTABLE ORDER follows dScene_c's (unchanged slot indices; dScStarSel_c
 * adds no new virtual). The destructor pair is at slots 16/17, same
 * convention as dScEntry_c and every other fBase_c-family class
 * (dScene_c.h/dScEntry_c.h): func_ov003_020addfc (D1) tears down the dExtSimpleModel_c
 * pair then writes dScene_c's/dBase_c's vptrs before fBase_c::~fBase_c;
 * func_ov003_020ade54 (D0) does the same then calls
 * Memory::Deallocate(this, GAME_HEAP_PTR) -- dScStarSel_c declares no
 * operator delete of its own, so this finds dScene_c's inline copy on its
 * immediate base, same as dScEntry_c.
 */
struct dScStarSel_c : dScene_c {
    /* Scale of the bottom-screen character face, handed to OAM::Render.
       It is an INVERSE scale: OAM::Render divides by it, so a bigger value
       draws the face smaller (width = 1/scale), and the tap animation in
       Render shrinks the face. Runtime (EU, blinded 12/12): 0x1800 drew the
       face 82 px wide and 0xa00 196 px, against 123 px at 0x1000. */
    s32 mFaceScaleX; /* 0x050 */
    s32 mFaceScaleY; /* 0x054 -- Render copies mFaceScaleX in */
    /* Scale of the picked character's icon in the strip, in the same
       OAM::Render slots as mFaceScaleX. Render's pick pulse writes it. */
    s32 mIconScaleX;             /* 0x058 */
    s32 mIconScaleY;             /* 0x05c */
    s32 mIdleIconScale;          /* 0x060 -- the other strip icons; 0 = not drawn */
    dExtSimpleModel_c models[2]; /* 0x064 -- dExtSimpleModel_c::LoadAndSetFile'd pair,
                                     see CONSTRUCTION above */
    /* D-pad auto-repeat timers: Behavior reloads them with 0x10 on a press
       and 8 while held, and counts them down. */
    u16 mPlateRepeatTimer; /* 0x104 -- the star plates */
    u16 mCharRepeatTimer;  /* 0x106 -- the character strip */
    /* The bounce of the bottom TOUCH ME! prompt (Behavior): mBounceDelay
       frames (0x78) pass, then mBounceVelocity is launched at mBounceLaunch
       (first -0x400) and steps mPromptYOffset until it lands. Each phase-3
       landing raises mBounceLaunch by 0x80 until it reaches -0x180 and the
       delay restarts. mBouncePhase is 1 above the rest line, 2 or 3 below. */
    u16 mBounceDelay; /* 0x108 */
    /* 8.8 y offset of the TOUCH ME! prompt. Runtime (EU, blinded 11/11):
       poking it moved the prompt. */
    s16 mPromptYOffset;  /* 0x10a */
    s16 mBounceVelocity; /* 0x10c */
    s16 mBounceLaunch;   /* 0x10e */
    /* Runtime (EU, blinded 8/12; both rotate pairs hit, and the four misses
       were all scale-direction guesses on 0x50): rotation of the face. */
    s16 mFaceAngle;  /* 0x110 */
    s16 mPulseAngle; /* 0x112 -- phase of the pick pulse on mIconScaleX */
    /* Runtime (EU, blinded 12/12): the number of star plates. */
    u8 mPlateCount; /* 0x114 */
    /* Runtime (EU, blinded 11/11): the highlighted plate, 0xff = none. */
    u8 mSelectedPlate; /* 0x115 */
    /* Index into OAM::LIFE_ICONS. Runtime (EU, blinded 11/11): the head
       icon of the lives counter. */
    u8 mLifeIcon; /* 0x116 */
    /* Set like mPickPressTimer when a plate is tapped or the selection
       moves, and counted down in Behavior. Nothing in the class reads it. */
    u8 mPlatePressTimer; /* 0x117 */
    u8 mPickPressTimer;  /* 0x118 -- frames the picked icon is drawn pressed */
    /* Set to 0x10 when a choice is made; Behavior counts it down and fades
       the scene out at zero. */
    u8 mExitDelay; /* 0x119 */
    /* x of plate i's number and its bottom-screen star icon. Runtime (EU,
       blinded 12/12); the 3D stars are centred by index and do not follow it. */
    u8 mPlateX[7]; /* 0x11a */
    s8 mDigits[3]; /* 0x121 -- func_ov003_020ae1a4's digits, -1 = blank */
    u8 mCharX[3];  /* 0x124 -- per character, in the strip */
    u8 pad_127;
    u8 mCharY[3]; /* 0x128 */
    /* Runtime (EU, blinded 8/12; both pairs hit): screen y of the face. */
    u8 mFaceY;         /* 0x12b */
    u8 mCharSprite[3]; /* 0x12c -- index into data_ov001_020abcb4 */
    u8 pad_12f;
    /* InitResources counts the unlocked characters into it; 4 = the fixed
       order in sStarSelectCharacterOrder. Runtime (EU, blinded 11/11): 2
       with character 3 hides the TOUCH ME! prompt and freezes its bounce. */
    u8 mStripMode; /* 0x130 */
    /* Bit i: plate i can be picked and has its small bottom-screen star
       icon. Runtime (EU, blinded 12/12): bit i draws that icon. */
    u8 mPlateMask;  /* 0x131 */
    u8 mPickedChar; /* 0x132 -- 3 = the face was chosen */
    /* What the pointer points at: 0 the plates, 1 the character strip,
       2 the face. Runtime (EU, blinded 11/11): 0 drew the down-pointer above
       the star icon, 2 the right-pointer left of the face. */
    u8 mCursorTarget; /* 0x133 */
    u8 mCharCursor;   /* 0x134 -- slot within the character strip */
    /* 0 hidden; 1 woken by the d-pad; 2 shown. Runtime (EU, blinded 11/11):
       nonzero draws the pointer. */
    u8 mCursorShown;  /* 0x135 */
    u8 mBouncePhase;  /* 0x136 */
    u8 mFaceAnimStep; /* 0x137 -- the tap's grow, shrink, wait, spin step */
    u8 pad_138;
    u8 mAnimMode; /* 0x139 -- 1 face chosen, 2 character picked */
    u8 mAnimFlip; /* 0x13a -- pick pulse: which icon side is drawn */
    u8 pad_13b;

    /* Declared first -- key function; see the family convention discussed in
       dBase_c.h/dScene_c.h. The D1 source now defines this destructor as a
       genuine compiler-spelled class method. The D0 source remains a real
       destructor definition from which mwccarm emits the deleting variant.
       Those one-function objects contribute only their selected text; the
       ov003 ROM-gap data remains the production owner of this class's vtable,
       RTTI and type-name objects. This establishes the ABI shape without
       claiming the exact original EAD translation-unit spelling. */
    virtual ~dScStarSel_c();                             /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dScene_c/_ZTV7fBase_c order. --- */
    virtual s32  InitResources();                        /* slot  0 */
    virtual s32  CleanupResources();                     /* slot  3 */
    virtual s32  Behavior();                              /* slot  6 */
    virtual s32  Render();                                /* slot  9 */
    virtual void OnPendingDestroy();                      /* slot 12 */

    /* Non-virtual. The address is the method name; no new vtable slot. Every
       inbound call in the class's run passes this, and every access in each
       body is a this-relative field of this class. */
    s32  func_ov003_020adec0(u32 chr);
    s32  func_ov003_020adf50();
    void func_ov003_020adfc8();
    void func_ov003_020ae0b0();
    void func_ov003_020ae1a4(s32 value);
    void func_ov003_020ae238();
    void func_ov003_020ae358();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScStarSel_c_size_must_be_0x13c[sizeof(dScStarSel_c) == 0x13c ? 1 : -1];
#endif

#endif
