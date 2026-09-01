/* class dScMgSlot1_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py
 * (zero RTTI edges name dScMgSlot1_c as a base).
 *
 * Own vtable slots (python tools/rtti_vtables.py --own dScMgSlot1_c): 0
 * (InitResources), 6 (Behavior -- declared, body not decompiled, see below),
 * 9 (Render), 16
 * (D1), 17 (D0), 18 (own new slot -- stays a raw extern "C" helper,
 * src/func_ov006_0210c674.c, same precedent as every other dScMgBase_c
 * leaf's slot 18), 27 (OnHitByMegaChar), 28 (OnHitFromUnderneath).
 * rtti_vtables.py --own also reports slot 36 (MgBingoBallSlotsShot_Spawn);
 * that one is SPURIOUS -- address-gap math confirms it: the vtable data
 * symbol ends exactly 0x90 bytes (36 words) before the unrelated
 * MgBingoBallSlotsShot_SpawnInfo table, which MgBingoBallSlotsShot_Spawn
 * itself uses to build a completely different, larger object with its own
 * vtable -- excluded, along with MgBingoBallSlotsShot_Spawn and anything
 * named "MgBingoBallSlotsShot", from this migration.
 *
 * slot 9's recovered symbol name (_ZN3OAM7SECONDSE, i.e. "OAM::SECONDS") is
 * a name-recovery-heuristic artifact, not a real name -- the function at
 * that address genuinely is Render (confirmed by vtable slot identity and
 * by matching every sibling's Render shape).
 *
 * BEHAVIOR (slot 6) IS DECLARED BUT NOT DECOMPILED. dScMgSlot1_c's own
 * Behavior -- 0x81c bytes at ov006:0x0210c9e0 -- has never been matched: its
 * .text range (0x0210c9e0..0x0210d1fc) has no entry anywhere in
 * config/arm9/overlays/ov006/delinks.txt, so it is served, unenrolled, by
 * the module's gap object straight from the ROM dump (tools/rombuild.py's
 * own banner: "every address range NOT enrolled is supplied by a delinked
 * gap object carrying the original ROM bytes"). There is still no C source
 * to convert -- that is original matching work, a different and much larger
 * task than this struct-to-class migration.
 *
 * The DECLARATION is nevertheless required, and an earlier revision of this
 * banner was wrong to say otherwise. It claimed "this class's own vtable
 * data, data_ov006_0213eb40, is never compiler-emitted either -- only
 * aliased -- so the real ROM vtable word for slot 6 is untouched". That
 * stopped being true once the destructor became this class's key function:
 * _ZTV12dScMgSlot1_c IS emitted, from src/_ZN12dScMgSlot1_cD1Ev.cpp, and
 * tools/romdata_check.py byte-compares it against the cartridge. With
 * Behavior undeclared mwcc wrote dScMgBase_c's own body (0x020b0618) into
 * slot 6 where the cartridge holds 0x0210c9e0, and the whole table scored
 * DIFFERS. Declaring it, and naming the ROM body in ov006/symbols.txt so the
 * slot has a symbol to point at, fixes that word without touching a byte of
 * code: the 0x81c unmatched bytes are still the cartridge's.
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
 * func_ov006_0210d6b8 remains an explicit C-ABI factory because this compiler
 * cannot reproduce the ROM factory with a usable class-specific or placement
 * operator new spelling (the documented factory wall). Its allocation size,
 * object pointer, member pointer and vtable symbols are nevertheless typed;
 * only the codegen-sensitive construction sequence remains explicit.
 *
 * 0x4684 and 0x4690 are two more embedded objects (0xc bytes each,
 * constructed via func_ov006_0210c2b0 in the constructor, ticked via
 * func_ov006_0210c234 in Render), and 0x469c is a further one (constructed
 * via func_ov006_0210c208, size unknown). None of InitResources/Render/D1/
 * D0 read or write inside any of the three directly -- only pass their
 * address to a helper -- so all three stay raw padding.
 *
 * Field at `this+8` some of these methods read is INHERITED from further up
 * the hierarchy than dScMgBase_c (dScene_c/fBase_c level, well below where
 * dScMgBase_c's own fields start at 0x50) -- accessed via raw offset
 * arithmetic on a char* cast, not added as a named field anywhere.
 *
 * 0x4706 is read only by the unmigrated slot-18 helper (itself called from
 * InitResources via a vtable-shim, but not a field InitResources/Render/D1/
 * D0 touch directly) -- same precedent dScMgPachinko2_c.h documents for its
 * own offset 0xbc: stays raw padding here, and the slot-18 helper reaches
 * it (and 0x46b4) via a raw offset cast rather than a named field, so it no
 * longer includes this header.
 *
 * slots 27/28 (OnHitByMegaChar/OnHitFromUnderneath) are real overrides.  The
 * names were independently recovered on dScMgBase_c's own copies at the same
 * two slots (src/_ZN11dScMgBase_c15OnHitByMegaCharEv.cpp and
 * .../_ZN11dScMgBase_c19OnHitFromUnderneathEv.cpp, both of which this class's
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
 * real slot-18 body) and 0x020b2994 (the base's OnTurnIntoEgg), and
 * _ZTV12dScMgSlot1_c scored DIFFERS on those two words.  It scores PARTIAL now
 * -- byte-exact for every word it emits.  That was the first DIFFERS the
 * slot-by-slot keystone campaign CLOSED rather than widened, and it was closed
 * by a change in a DIFFERENT header, which is the whole reason romdata_check
 * runs on every slot of this campaign: rombuild was green before and after.
 *
 * OnHitByMegaChar became a genuine override when the base declared slot 27,
 * and OnHitFromUnderneath becomes one here.  dScMgBase_c now declares 18
 * through 28 without a gap, so the base fixes every index in this class and
 * mwcc numbers no virtual of it for itself any more.
 *
 * THE ARITHMETIC WAS LOAD-BEARING RIGHT UP TO THIS COMMIT, and the trap the
 * paragraph here used to warn about is the one this commit walked through.
 * OnHitFromUnderneath sat on 28 only because the base stopped at 27 and 28 was
 * the next free index.  Had the base declared `OnHitFromUnderneath()` while
 * this header kept `(dActor_c &)`, the two would have been different
 * functions: the base's takes 28, this one becomes a new slot at 29, and
 * _ZTV12dScMgSlot1_c goes straight back to DIFFERS -- with rombuild green
 * throughout, because rombuild does not read vtables.  Both declarations
 * changed together and romdata_check is what says they were reconciled.
 *
 * Slots 29-35 add no virtual to this class, so the trap cannot recur HERE.  It
 * recurs anywhere else a descendant declared a base slot early, which is the
 * one thing to check before declaring each of the seven that remain. */
#ifndef DSCMGSLOT1_C_H
#define DSCMGSLOT1_C_H
#include "dScMgBase_c.h"

struct dScMgSlot1_c : dScMgBase_c {
    struct betIcon_c : dThIcon_c {
        betIcon_c() {}
        ~betIcon_c() {}
        virtual void Behavior();
        virtual void Render();
    };

    virtual ~dScMgSlot1_c();
    virtual s32 InitResources();                       /* slot  0 */
    virtual s32 Behavior();     /* slot  6 -- ov006 0x0210c9e0, not decompiled */
    virtual s32 Render();                               /* slot  9 */
    /* All three override dScMgBase_c -- slots 18, 27 and 28 -- so the base
       fixes every index below and declaration order no longer decides any of
       them.  It decided 27 and 28 until the base declared them; see the banner
       for what that cost and how it was closed. */
    virtual int  OnYoshiTryEat(int arg);                 /* slot 18 */
    virtual void OnHitByMegaChar();                     /* slot 27 */
    virtual void OnHitFromUnderneath();                 /* slot 28 */

    betIcon_c mBetIcon;      /* 0x4660 -- RTTI-proven nested touch icon */
    u8  pad_4684[0xc];      /* 0x4684 -- embedded object, own type unknown */
    u8  pad_4690[0xc];      /* 0x4690 -- embedded object, own type unknown */
    u8  pad_469c[0x8];      /* 0x469c -- embedded object, own type unknown */
    u32 unk_46a4[3];         /* 0x46a4 */
    u8  pad_46b0[0x4];
    s32 unk_46b4;            /* 0x46b4 */
    u8  pad_46b8[0x4];
    s32 unk_46bc;            /* 0x46bc */
    u8  unk_46c0[0x15];      /* 0x46c0 */
    u8  pad_46d5[0x2a];
    u8  unk_46ff[3];         /* 0x46ff */
    u8  unk_4702[3];         /* 0x4702 */
    u8  pad_4705[0x4];       /* covers 0x4706, touched only by the
                                 unmigrated slot 18 helper -- see banner */
    u8  unk_4709;            /* 0x4709 */
    u8  unk_470a;            /* 0x470a */
    u8  unk_470b;            /* 0x470b */
    u8  pad_470c[0x4];
};

typedef char dScMgSlot1_c_size_must_be_0x4710[sizeof(dScMgSlot1_c) == 0x4710 ? 1 : -1];

#endif
