/* Hand-written from matched-function evidence:
 * class daCamTag_c, ov002 0x020b0710-0x020b07c8 (8 functions, no other class
 * in the TU -- tu_map.py).
 *
 * ALL FIVE of its virtual overrides are STUBS -- InitResources, Behavior,
 * Render and CleanupResources are `return 1`, OnPendingDestroy is empty, and
 * not one of them reads `this`. The class exists to BE somewhere: it is a
 * dActor_c and nothing more functionally, marking a position other code
 * queries.
 *
 * SIZE 0xd4 (212), tools/opnew_sizes.py's own literal from daCamTag_c_Spawn's
 * `operator new` call -- independent ROM evidence, not the header's own
 * sizeof() echoed back. dActor_c itself is 0xd0 (include/dActor_c.h), so
 * daCamTag_c adds exactly FOUR bytes on top of it: one unknown/unused field,
 * not zero as an earlier version of this header claimed.
 *
 *   daCamTag_c_Spawn  0x020b07c8  new(212 == 0xd4), dActor_c::dActor_c(),
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
 * ROM RTTI calls this class daCamTag_c (_ZTS10daCamTag_c /
 * config/arm9/overlays/ov002/symbols.txt) -- a different length than the
 * decomp's daCamTag_c, so this class's own _ZTI/_ZTS objects are not
 * byte-verifiable against the ROM's; left unlicensed, deferred to Phase F /
 * attribution work like every other class in this position.
 *
 * Field NAMES are placeholders - renaming cannot change codegen.
 */
#ifndef DACAMTAG_C_H
#define DACAMTAG_C_H
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
