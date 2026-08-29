/* class dScMgSlot1_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py
 * (zero RTTI edges name dScMgSlot1_c as a base).
 *
 * Own vtable slots (python tools/rtti_vtables.py --own dScMgSlot1_c): 0
 * (InitResources), 6 (Behavior -- NOT migrated, see below), 9 (Render), 16
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
 * BEHAVIOR (slot 6) IS NOT MIGRATED, unlike every other dScMgBase_c leaf on
 * this branch. dScMgSlot1_c's own Behavior -- func_ov006_0210c9e0, 0x81c
 * bytes -- has never been decompiled: its .text range (0x0210c9e0..
 * 0x0210d1fc) has no entry anywhere in
 * config/arm9/overlays/ov006/delinks.txt, so it is served, unenrolled, by
 * the module's gap object straight from the ROM dump (tools/rombuild.py's
 * own banner: "every address range NOT enrolled is supplied by a delinked
 * gap object carrying the original ROM bytes"). There is no C source to
 * convert -- this is original matching work, a different and much larger
 * task than this struct-to-class migration. dScMgBase_c declares
 * `virtual s32 Behavior();` without a body, so simply not re-declaring it
 * here leaves that inherited declaration in place; nothing in this
 * migration needs its address resolved (this class's own vtable data,
 * data_ov006_0213eb40, is never compiler-emitted either -- only aliased,
 * see the rename commit -- so the real ROM vtable word for slot 6 is
 * untouched and keeps pointing at func_ov006_0210c9e0 exactly as before).
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
 * slots 27/28 (OnHitByMegaChar/OnHitFromUnderneath) are real overrides,
 * with the exact signature copied from include/dActor_c.h's own slots 27/28 --
 * the same "OnHitByMegaChar"/"OnHitFromUnderneath" names were independently
 * recovered on dScMgBase_c's own copy at the same two slots
 * (src/func_ov004_020af27c.cpp and .../func_ov004_020af04c.cpp, both of
 * which dScMgSlot1_c's own overrides call into), and on many unrelated
 * fBase_c descendants across other overlays -- a shared, fixed
 * collision-event slot pair used across both the dActor_c and dScene_c branches,
 * not a coincidence of numbering. Neither dScMgBase_c.h nor dScene_c.h/
 * fBase_c.h currently declares a slot at 27/28 for the dScene_c branch (they
 * are left undeclared, same as dScMgBase_c.h's own "18-35... left
 * undeclared" note), so the compiler's OWN internal slot assignment for
 * these two methods in THIS class does not literally land on index 27/28 --
 * only the ROM's real vtable (never compiler-emitted for this class, see
 * above) does. The comments below record the true ROM position for a human
 * reader, same as every other slot-number comment in this file. */
#ifndef DSCMGSLOT1_C_H
#define DSCMGSLOT1_C_H
#include "dScMgBase_c.h"

struct Player;
struct dActor_c;

struct dScMgSlot1_c : dScMgBase_c {
    struct betIcon_c : dThIcon_c {
        betIcon_c() {}
        ~betIcon_c() {}
        virtual void Behavior();
        virtual void Render();
    };

    virtual ~dScMgSlot1_c();
    virtual s32 InitResources();                       /* slot  0 */
    virtual s32 Render();                               /* slot  9 */
    virtual void OnHitByMegaChar(Player &player);       /* slot 27 -- void, see include/Stump.h */
    virtual int OnHitFromUnderneath(dActor_c &other);      /* slot 28 */

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
