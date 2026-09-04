#ifndef DAOBJCLOCKHURIKO_C_H
#define DAOBJCLOCKHURIKO_C_H

#include "types.h"

#ifdef __cplusplus
#include "dActor_c.h"
#include "Model.h"
#endif

/* The swinging pendulum inside the clock painting -- a leaf dActor_c child at
 * ov013 0x021111a0..0x02111384, alongside its sibling ClockPaintingHandShort
 * (include/ClockPaintingHandShort.h, a separate class -- same painting, not a
 * base/derived relationship: neither RTTI record names the other).
 *
 * DERIVATION. The RTTI record at ov013 0x021120e0 is mangled
 * "18daObjClockHuriko_c" -- the ROM struct name (`huriko` is the ROM's own
 * word for pendulum) -- with ONE base, dActor_c, at subobject offset 0. Read
 * straight out of extracted/overlays/overlay_0013.bin, not inferred: the
 * _ZTI's three words are {0x0209a764 __si_class_type_info+8, 0x021120ec ->
 * the "18daObjClockHuriko_c" string, 0x0208e390 = _ZTI8dActor_c}, so the
 * cartridge names the base itself. It is a leaf: nothing in the image derives
 * from it. include/decl_common.h:649 has long carried
 * `extern int _ZTV18daObjClockHuriko_c[];`; that declaration was dangling
 * while the vtable was still recorded under the coined spelling, and the
 * rename to the cartridge name is what finally gave it its definition at
 * ov013 0x02112128 (the address point -- the table itself starts 8 bytes
 * earlier at 0x02112120, behind offset-to-top and the _ZTI pointer).
 *
 * VTABLE. _ZTV18daObjClockHuriko_c is 31 slots, the same count as
 * dActor_c's own table. That is not just a tool's reading any more:
 * romdata_check word-compares the table this TU emits against the cartridge
 * and reports it VERIFIED at 124 bytes -- 31 slots exactly, the whole table,
 * neither truncated nor overrun. The destructor pair sits at ov013
 * 0x021111a0 (D1) / 0x021111d0 (D0). This class overrides four slots beyond
 * the destructor, all four now real methods in
 * src/actors/d_a_obj_clock_huriko.cpp:
 *
 *   0  InitResources      ov013 0x0211133c   <- the key function, see below
 *   3  CleanupResources   ov013 0x02111214
 *   6  Behavior           ov013 0x021112a8
 *   9  Render             ov013 0x02111280
 *
 * (config/arm9/overlays/ov013/relocs.txt has no entries at the vtable's own
 * slot addresses because the whole vtable is one relocation-free literal
 * data blob in this overlay; the four function addresses above are exactly
 * what tools/rtti_vtables.py read out of that data, cross-checked against
 * config/arm9/overlays/ov013/symbols.txt's function addresses.) Slot 12
 * (OnPendingDestroy) is NOT overridden -- unlike Door (include/Door.h), which
 * overrides all five of InitResources/CleanupResources/Behavior/Render/
 * OnPendingDestroy, this class leaves slot 12 pointing at fBase_c's
 * implementation, so it is not part of this recovery.
 *
 * All four bytes still match; the symbol names were placeholders
 * (func_ov013_0211xxxx) before the rename. func_ov013_02111238, the small
 * helper both InitResources and Behavior call, now lives in the same TU but
 * stays a free function under its func_ name -- it is not a vtable slot, and
 * nothing in the cartridge says whether it is a member.
 *
 * REAL METHODS, NOT THE FREE-FUNCTION IDIOM. Older recoveries (include/Door.h,
 * src/_ZN7fBase_c13InitResourcesEv.cpp) declare the override here but define
 * it as a free function under a hand-written mangled `extern "C"` name. That
 * idiom cannot survive promotion. With the destructor inline the key function
 * is the first DECLARED non-inline virtual -- InitResources -- and a
 * hand-mangled free function does not DEFINE it, so mwcc emits neither the
 * _ZTV/_ZTI/_ZTS group nor the inline destructor's D1/D0 pair, and objisolate
 * refuses the entry with `_ZN18daObjClockHuriko_cD1Ev has 0 defined symbols`
 * (the same failure src/actors/d_a_tree.cpp records at its own line 133). All
 * four overrides are therefore real `daObjClockHuriko_c::` methods, which is
 * what makes this TU the key-function TU. Bodies are unchanged apart from the
 * implicit `this`; the ROM bytes still match at 106/106.
 *
 * SIZE. daObjClockHuriko_c_Spawn.c calls `_ZN7fBase_cnwEj(296)` -- 0x128 --
 * then _ZN8dActor_cC2Ev and _ZN5ModelC1Ev at +0xd4. dActor_c is 0xd0
 * (include/dActor_c.h) and Model is 0x50 (include/Model.h), so the embedded
 * Model runs 0xd4..0x124 (the same 4-byte alignment pad include/dBgActor_c.h
 * and include/Door.h both take before their own Model/ModelAnim members).
 * That leaves exactly 0x124..0x127 (4 bytes) as this class's own storage --
 * one s16, read and written as `*(short *)(c + 0x124)` in both
 * InitResources (set to 0x100) and Behavior (nudged by 8 per frame, compared
 * against +-0x10), plus 2 bytes of trailing padding. Same offset,
 * ClockPaintingHandShort.h's sibling field (`mHandIndex`, u8) confirms the
 * shape -- one small scalar right after the embedded Model.
 *
 * Field NAMES are placeholders and cannot change codegen. Offsets and widths
 * are observed.
 *
 * SM64DS RTTI names the implementation daObjClockHuriko_c. The reconstructed factory
 * daObjClockHuriko_c_classInit (historical alias daObjClockHuriko_c_Spawn) installs this class's
 * cartridge vtable for the CLOCK_HURIKO registry profile.
 */

#ifdef __cplusplus

struct daObjClockHuriko_c : dActor_c {
    u8  pad_0d0[0x4];
    /* Named by daObjClockHuriko_c_Spawn.c's own _ZN5ModelC1Ev call at
       +0xd4 -- a relocation the ROM build checks. */
    Model mModel;             /* 0x0d4 */

    s16 mAngSpeed;               /* 0x124 -- swing angle/phase, see SIZE above */
    u8  pad_126[0x2];

    /* --- vtable. The destructor is INLINE, and that is load-bearing rather
       than a style choice. Out of line, mwccarm emits D0 before D1 -- the
       reverse of the ROM's 0x021111a0 D1 / 0x021111d0 D0 order, which makes
       objisolate refuse the whole TU -- and additionally emits a D2 that has
       no home anywhere in the cartridge. Inline, it emits exactly the retail
       D1/D0 pair in ROM order and no D2. Nothing derives from this class
       (it is a leaf, see DERIVATION), so no descendant needs to `bl` a D2.

       Declared first, deliberately: with the destructor inline the key
       function is the first DECLARED non-inline virtual, so this ordering is
       what makes src/actors/d_a_obj_clock_huriko.cpp the TU that emits the
       _ZTV/_ZTI/_ZTS group -- exactly what the promotion needs it to be. --- */
    virtual ~daObjClockHuriko_c() {}

    /* --- overrides of inherited fBase_c slots dActor_c left untouched.
       Now real daObjClockHuriko_c:: methods, defined in
       src/actors/d_a_obj_clock_huriko.cpp; the earlier free-function-under-a-
       mangled-symbol idiom is gone with the promotion. --- */
    virtual s32 InitResources();          /* slot 0 */
    virtual s32 CleanupResources();       /* slot 3 */
    virtual s32 Behavior();               /* slot 6 */
    virtual s32 Render();                 /* slot 9 */
};

/* Holds the chain to the size daObjClockHuriko_c_Spawn.c's
   operator new(0x128) call evidences. A silently-added member anywhere
   fails this. */
typedef char daObjClockHuriko_c_size_must_be_0x128[sizeof(daObjClockHuriko_c) == 0x128 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJCLOCKHURIKO_C_H */
