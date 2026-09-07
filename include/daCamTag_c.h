/* Hand-written from matched-function evidence:
 * class daCamTag_c, ov002 0x020b0748-0x020b07f8 (8 functions, no other class
 * in the TU -- tu_map.py). That range is the entry's licensed `complete`
 * delinks span and it is exact: the eight functions chain contiguously from
 * 0x020b0748 up to daCamTag_c_classInit at 0x020b07c8 + 0x30. An earlier version
 * of this line said 0x020b0710-0x020b07c8, which started one function too
 * early -- 0x020b0710 is InvisiblePole_Spawn, the same off-by-one factory
 * that produced the retracted layout described below.
 *
 * ALL FIVE of its virtual overrides are STUBS -- InitResources, Behavior,
 * Render and CleanupResources are `return 1`, OnPendingDestroy is empty, and
 * not one of them reads `this`. The class exists to BE somewhere: it is a
 * dActor_c and nothing more functionally, marking a position other code
 * queries.
 *
 * SIZE 0xd4 (212), tools/opnew_sizes.py's own literal from daCamTag_c_classInit's
 * `operator new` call -- independent ROM evidence, not the header's own
 * sizeof() echoed back. dActor_c itself is 0xd0 (include/dActor_c.h), so
 * daCamTag_c adds exactly FOUR bytes on top of it: one unknown/unused field,
 * not zero as an earlier version of this header claimed.
 *
 *   daCamTag_c_classInit  0x020b07c8  new(212 == 0xd4), dActor_c::dActor_c(),
 *                                stores _ZTV10daCamTag_c.
 *
 * THIS HEADER USED TO SAY 0x108, with a dCcAc_c at 0xd4. That was
 * daBar_c's layout, read off daBar_c_classInit (0x020b0710; historical alias
 * InvisiblePole_Spawn) -- the
 * function immediately BEFORE this class's own factory, whose story the old
 * comment reproduced verbatim, down to naming it. daBar_c is a different
 * class with a different vtable (0x02108480, RTTI daBar_c), and it is already
 * correctly declared at 0x108 in include/daBar_c.h, dCcAc_c and all.
 * Pairing a class to a factory by NAME is what produced that, and it is the
 * same defect PR #1556 had to retract two findings for. tools/opnew_sizes.py
 * pairs by vtable address instead, and that is what caught this one.
 *
 * THE VTABLE, at ov002 0x0210853c (config/arm9/overlays/ov002/symbols.txt),
 * overrides the same five fBase_c slots daBar_c does -- InitResources
 * (0), CleanupResources (3), Behavior (6), Render (9), OnPendingDestroy (12)
 * -- plus the destructor at 16/17. Every other slot holds dActor_c's own word
 * and is inherited, so it is deliberately NOT redeclared here.
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN, and that is what makes this class's
 * RTTI byte-verifiable. The decomp used to coin it `CameraTag`, whose
 * _ZTI/_ZTS records could never be compared against the ROM -- a typeinfo
 * record is a length-prefixed mangled string, so a coined name misses on both
 * the prefix and the body. Renamed to daCamTag_c, they compare exactly:
 * _ZTS10daCamTag_c at ov002:0x02108508 is the byte string "10daCamTag_c" in
 * extracted/overlays/overlay_0002.bin, and _ZTI10daCamTag_c at 0x021084fc
 * reads [0x0209a764, 0x02108508, 0x0208e390] -- __si_class_type_info's
 * vtable, that typeinfo name, and _ZTI8dActor_c, so the cartridge states the
 * direct base as well. All three records are licensed as `deadstrip-data`
 * rows in config/tu_manifest.d/ov002/daCamTag_c.json and romdata_check
 * word-compares them against the cartridge.
 *
 * Field NAMES are placeholders - renaming cannot change codegen.
 */
#ifndef DACAMTAG_C_H
#define DACAMTAG_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daCamTag_c -- daCamTag_c_classInit (was daCamTag_c_Spawn),
 *       g_profile_CAMERA_TAG (was daCamTag_c_SpawnInfo)
 */
#include "types.h"

#ifdef __cplusplus
#include "dActor_c.h"

struct daCamTag_c : dActor_c {
    u8 unk_0d0[0x4];                  /* 0x0d0 */

    /* Defined inline on purpose. InitResources is the first out-of-line
     * virtual/key function, so together with this inline destructor mwcc owns the
     * retail D1/D0 pair in the cartridge's own order and the complete class
     * RTTI/vtable group, without retaining a D2 base-object body that has no ROM
     * home. Written out-of-line, mwcc emits D0 before D1 and objisolate refuses
     * the translation unit. The body is empty in the ROM too: D1 at 0x020b0748
     * stores the vtable and tail-calls the base destructor, with no member
     * teardown. */
    virtual ~daCamTag_c() {}           /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();     /* slot  0 */
    virtual s32  CleanupResources();  /* slot  3 */
    virtual s32  Behavior();          /* slot  6 */
    virtual s32  Render();            /* slot  9 */
    virtual void OnPendingDestroy();  /* slot 12 */
};

typedef char daCamTag_c_size_must_be_0xd4[sizeof(daCamTag_c) == 0xd4 ? 1 : -1];

#else

/* Flat layout for the C translation units, which can express neither the
   base class nor the virtual functions. */
struct daCamTag_c {
    u8  pad_000[0xd4];
};

#endif /* __cplusplus */

#endif
