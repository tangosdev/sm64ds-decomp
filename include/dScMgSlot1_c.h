/* class dScMgSlot1_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py
 * (zero RTTI edges name dScMgSlot1_c as a base).
 *
 * Own vtable slots (python tools/rtti_vtables.py --own dScMgSlot1_c): 0
 * (InitResources), 6 (Behavior),
 * 9 (Render), 16
 * (D1), 17 (D0), 18 (StartRound), 27 (OpenMenu), and
 * 28 (CloseMenu).
 * rtti_vtables.py --own also reports slot 36 (dScMgSmartball_c_classInit);
 * that one is SPURIOUS -- address-gap math confirms it: the vtable data
 * symbol ends exactly 0x90 bytes (36 words) before the unrelated
 * g_profile_MG_SMARTBALL table, which dScMgSmartball_c_classInit
 * itself uses to build a completely different, larger object with its own
 * vtable -- excluded, along with dScMgSmartball_c_classInit and anything
 * named "MgBingoBallSlotsShot", from this migration.
 *
 * slot 9's recovered symbol name (_ZN3OAM7SECONDSE, i.e. "OAM::SECONDS") is
 * a name-recovery-heuristic artifact, not a real name -- the function at
 * that address genuinely is Render (confirmed by vtable slot identity and
 * by matching every sibling's Render shape).
 *
 * Behavior (ov006:0x0210c9e0..0x0210d1fc, 0x81c bytes) is byte-exact since
 * 2026-10-12. The 19-word residue that notes/mwccarm-codegen.md section 6cm
 * banked as a compiler floor was a source-shape artefact: the draft named
 * the object two ways (members and a char* with literal offsets), and that
 * is what made 2004/b56 fold the hoisted touch-record address. Spelled
 * through members alone it reproduces the cartridge.
 *
 * The DECLARATION is nevertheless required, and an earlier revision of this
 * banner was wrong to say otherwise. It claimed "this class's own vtable
 * data, data_ov006_0213eb40, is never compiler-emitted either -- only
 * aliased -- so the real ROM vtable word for slot 6 is untouched". That
 * stopped being true once the destructor became this class's key function:
 * _ZTV12dScMgSlot1_c IS emitted, from src/minigames/d_s_mg_slots.cpp, and
 * tools/romdata_check.py byte-compares it against the cartridge. With
 * Behavior undeclared mwcc wrote dScMgBase_c's own body (0x020b0618) into
 * slot 6 where the cartridge holds 0x0210c9e0, and the whole table scored
 * DIFFERS. Declaring it, and naming the ROM body in ov006/symbols.txt so the
 * slot has a symbol to point at, fixes that word without touching a byte of
 * code: the matching 0x81c-byte Behavior body is now enrolled.
 *
 * THE EMBEDDED SUBOBJECT AT 0x4660. build/rtti.json proves the distinct
 * nested class `dScMgSlot1_c::betIcon_c` (N12dScMgSlot1_c9betIcon_cE,
 * ov006:0x0213e594) single-inherits dThIcon_c at offset zero. The ROM's two
 * vtable entries identify the inherited interface as Behavior then Render;
 * betIcon_c overrides both. Constructor and destructor code establish the
 * ordinary base-then-derived / derived-then-base vtable write order, while
 * the next object at 0x4684 fixes betIcon_c's size at 0x24. It is therefore
 * represented as the typed member mBetIcon. Empty C++ constructors and
 * destructors emit those vtable writes exactly; dScMgSlot1_c's own empty
 * destructor now lets the compiler perform the member teardown. Render calls
 * mBetIcon.Render() directly and retains the ROM's virtual dispatch.
 *
 * The constructor is inline in src/minigames/d_s_mg_slots.cpp, so
 * dScMgSlot1_c_classInit, a plain `new dScMgSlot1_c`, holds it: the base
 * constructor, mBetIcon, and the three marker clears.
 *
 * 0x4684 and 0x4690 are two SlotBlink markers (0xc bytes each) and 0x469c
 * a SlotLamp (0x8 bytes): the constructor clears them through SlotBlink_Clear
 * and SlotLamp_Clear, Behavior starts and ticks them, and Render draws the two
 * blinks. The class methods only pass their addresses to those helpers; the
 * helpers' own stores and reads give the two structs their fields.
 *
 * The word at `this+8` InitResources reads is fBase_c::param1, inherited
 * from well below dScMgBase_c's own fields; its low byte picks the
 * alternative reel strips.
 *
 * slots 27/28 (OpenMenu/CloseMenu) are real overrides.  The
 * names were independently recovered on dScMgBase_c's own copies at the same
 * two slots (dScMgBase_c::OpenMenu and ::CloseMenu in
 * src/minigames/d_s_mg_base.cpp, both of which this class's
 * overrides call into), and on many unrelated fBase_c descendants across other
 * overlays -- a shared, fixed collision-event slot pair used across both the
 * dActor_c and dScene_c branches, not a coincidence of numbering.
 *
 * THE SIGNATURES ARE NOT dActor_c.h's, whatever this banner used to say.  Both
 * were copied from include/dActor_c.h's slots 27/28 back when nothing in this
 * branch declared them, and both have now come off.  Slot 27's `Player &` went
 * when the base declared 27: neither body reads a second argument register.
 * Slot 28's `dActor_c &other` goes in this commit, on dScMgBase_c's evidence
 * alone -- its body at ov004:0x020af04c writes r1 (`add r1, r4, #0x4000`)
 * before it ever reads it and touches no other argument register.
 *
 * That last one is measured ONCE, not twice, and the difference is worth
 * keeping in view.  At slot 27 this class's own override corroborated the
 * base: it zeroed r0/r1 for SetSubBg1Offset before doing anything else.  The
 * slot-28 override cannot, because it calls the base as its very first act --
 * a second argument would ride through r1 untouched and leave no trace either
 * way.  What the cartridge does gate is that the two declarations AGREE, and
 * romdata_check is the gate that sees it.
 *
 * THE MISLANDING THIS PARAGRAPH USED TO DESCRIBE IS FIXED.  Declaring
 * dScMgBase_c's slot 26 fixed half of it and slot 27 -- this commit -- fixed
 * the rest.  Before slot 26 these two took the compiler's 26 and 27 and wrote
 * 0x0210c4dc and 0x0210c4b8 where the cartridge holds 0x0210c674 (this class's
 * real slot-18 body) and 0x020b2994 (the base's CanEndState), and
 * _ZTV12dScMgSlot1_c scored DIFFERS on those two words.  It scores PARTIAL now
 * -- byte-exact for every word it emits.  That was the first DIFFERS the
 * slot-by-slot keystone campaign CLOSED rather than widened, and it was closed
 * by a change in a DIFFERENT header, which is the whole reason romdata_check
 * runs on every slot of this campaign: rombuild was green before and after.
 *
 * OpenMenu became a genuine override when the base declared slot 27,
 * and CloseMenu becomes one here.  dScMgBase_c now declares 18
 * through 28 without a gap, so the base fixes every index in this class and
 * mwcc numbers no virtual of it for itself any more.
 *
 * THE ARITHMETIC WAS LOAD-BEARING RIGHT UP TO THIS COMMIT, and the trap the
 * paragraph here used to warn about is the one this commit walked through.
 * CloseMenu sat on 28 only because the base stopped at 27 and 28 was
 * the next free index.  Had the base declared `CloseMenu()` while
 * this header kept `(dActor_c &)`, the two would have been different
 * functions: the base's takes 28, this one becomes a new slot at 29, and
 * _ZTV12dScMgSlot1_c goes straight back to DIFFERS -- with rombuild green
 * throughout, because rombuild does not read vtables.  Both declarations
 * changed together and romdata_check is what says they were reconciled.
 *
 * Slots 29-35 add no virtual to this class, so the trap cannot recur HERE.  It
 * recurs anywhere else a descendant declared a base slot early, which is the
 * one thing to check before declaring each of the seven that remain.
 *
 * SM64DS RTTI names the implementation dScMgSlot1_c. The reconstructed factory
 * dScMgSlot1_c_classInit (historical alias func_ov006_0210d6b8) installs this class's
 * cartridge vtable for the MG_SLOT1 registry profile.
 */
#ifndef DSCMGSLOT1_C_H
#define DSCMGSLOT1_C_H
#include "dScMgBase_c.h"

/* A blinking win-line marker, 0xc bytes: SlotBlink_Clear zeroes it,
 * SlotBlink_Start places and arms it, SlotBlink_Tick counts it down and
 * SlotBlink_Draw draws it. */
struct SlotBlink {
    s16 mX;      /* 0x0 -- SlotBlink_Start stores; SlotBlink_Draw draws at it */
    s16 mY;      /* 0x2 */
    s32 mTimer;  /* 0x4 -- SlotBlink_Start loads 0x48; SlotBlink_Tick counts it down */
    u8 mVisible; /* 0x8 -- flipped every 8 ticks; SlotBlink_Draw draws only while set */
};

/* The payout lamp, 0x8 bytes: SlotLamp_Clear zeroes it, SlotLamp_Start arms
 * it and SlotLamp_Tick counts it down, mirroring mOn into bit 1 of
 * data_0209d454. */
struct SlotLamp {
    s32 mTimer; /* 0x0 -- SlotLamp_Start loads 0x48; SlotLamp_Tick counts it down */
    u8 mOn;     /* 0x4 -- flipped every 8 ticks */
};

struct dScMgSlot1_c : dScMgBase_c {
    struct betIcon_c : dThIcon_c {
        betIcon_c() {}
        ~betIcon_c() {}
        virtual void Behavior();
        virtual void Render();
    };

    dScMgSlot1_c();
    virtual ~dScMgSlot1_c();
    virtual s32 InitResources(); /* slot  0 */
    virtual s32 Behavior();      /* slot  6 -- ov006 0x0210c9e0, byte-exact */
    virtual s32 Render();        /* slot  9 */
    /* All three override dScMgBase_c -- slots 18, 27 and 28 -- so the base
       fixes every index below and declaration order no longer decides any of
       them.  It decided 27 and 28 until the base declared them; see the banner
       for what that cost and how it was closed. */
    virtual void StartRound(int arg); /* slot 18 */
    virtual void OpenMenu();          /* slot 27 */
    virtual int CloseMenu();          /* slot 28 */

    betIcon_c mBetIcon;    /* 0x4660 -- RTTI-proven nested touch icon */
    SlotBlink mMatchBlink; /* 0x4684 -- placed from sSlotMatchBlinkX/Y[mMatchedSymbol] */
    SlotBlink mWildBlink;  /* 0x4690 -- placed from sSlotWildBlinkX/Y[mWildCount - 1] */
    SlotLamp mPayoutLamp;  /* 0x469c -- started when a spin pays out */
    u32 mReelScroll[3];    /* 0x46a4 -- Fix12 distance to the next symbol */
    s32 mSpinSoundHandle;  /* 0x46b0 -- returned by Sound_PlayIfNotActive */
    s32 mState;            /* 0x46b4 -- Behavior's state-machine index */
    s32 mPayoutMultiplier; /* 0x46b8 -- matched-symbol and wild awards per bet */
    s32 mStartCredits;     /* 0x46bc */
    u8 mReelStrip[3][21];  /* 0x46c0 -- symbol strips, one 21-byte row per reel */
    u8 mSymbolIndex[3];    /* 0x46ff -- current strip index, modulo 21 */
    u8 mReelSpinning[3];   /* 0x4702 -- cleared by a touch on that reel */
    u8 mStoppedReelCount;  /* 0x4705 -- reels stopped by touch this spin */
    s8 mPrevMatchedSymbol; /* 0x4706 -- Slot1_InitHud seeds -1, StartRound(4) copies
                              mMatchedSymbol; nothing here reads it back */
    u8 pad_4707[0x1];      /* 0x4707 -- Slot1_InitHud stores 1; no reader */
    u8 mSpeedTableOffset;  /* 0x4708 -- added to reel index in the speed table */
    u8 mMatchedSymbol;     /* 0x4709 -- common non-wild symbol, or 5 if all wild */
    u8 mHasMatch;          /* 0x470a -- all three symbols agree, allowing wilds */
    u8 mWildCount;         /* 0x470b -- number of symbol-5 wilds */
    u8 mStateTimer;        /* 0x470c -- countdown via DecIfAbove0_Byte */
    u8 pad_470d[0x3];
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgSlot1_c_size_must_be_0x4710[sizeof(dScMgSlot1_c) == 0x4710 ? 1 : -1];
#endif

#endif
