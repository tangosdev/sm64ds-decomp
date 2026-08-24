/* Hand-written from matched-function evidence:
 * class CameraTag, ov002 0x020b0710-0x020b07c8 (8 functions, no other class
 * in the TU -- tu_map.py).
 *
 * ALL FIVE of its virtual overrides are STUBS -- InitResources, Behavior,
 * Render and CleanupResources are `return 1`, OnPendingDestroy is empty, and
 * not one of them reads `this`. The class exists to BE somewhere: it is a
 * dActor_c and nothing more functionally, marking a position other code
 * queries.
 *
 * SIZE 0xd4 (212), tools/opnew_sizes.py's own literal from CameraTag_Spawn's
 * `operator new` call -- independent ROM evidence, not the header's own
 * sizeof() echoed back. dActor_c itself is 0xd0 (include/dActor_c.h), so
 * CameraTag adds exactly FOUR bytes on top of it: one unknown/unused field,
 * not zero as an earlier version of this header claimed.
 *
 *   CameraTag_Spawn  0x020b07c8  new(212 == 0xd4), dActor_c::dActor_c(),
 *                                stores _ZTV9CameraTag.
 *
 * THIS HEADER USED TO SAY 0x108, with a dCcAc_c at 0xd4. That was
 * InvisiblePole's layout, read off InvisiblePole_Spawn (0x020b0710) -- the
 * function immediately BEFORE this class's own factory, whose story the old
 * comment reproduced verbatim, down to naming it. InvisiblePole is a different
 * class with a different vtable (0x02108480, RTTI daBar_c), and it is already
 * correctly declared at 0x108 in include/InvisiblePole.h, dCcAc_c and all.
 * Pairing a class to a factory by NAME is what produced that, and it is the
 * same defect PR #1556 had to retract two findings for. tools/opnew_sizes.py
 * pairs by vtable address instead, and that is what caught this one.
 *
 * THE VTABLE, at ov002 0x0210853c (config/arm9/overlays/ov002/symbols.txt),
 * overrides the same five fBase_c slots InvisiblePole does -- InitResources
 * (0), CleanupResources (3), Behavior (6), Render (9), OnPendingDestroy (12)
 * -- plus the destructor at 16/17. Every other slot holds dActor_c's own word
 * and is inherited, so it is deliberately NOT redeclared here.
 *
 * ROM RTTI calls this class daCamTag_c (_ZTS10daCamTag_c /
 * config/arm9/overlays/ov002/symbols.txt) -- a different length than the
 * decomp's CameraTag, so this class's own _ZTI/_ZTS objects are not
 * byte-verifiable against the ROM's; left unlicensed, deferred to Phase F /
 * attribution work like every other class in this position.
 *
 * Field NAMES are placeholders - renaming cannot change codegen.
 */
#ifndef CAMERATAG_H
#define CAMERATAG_H
#include "types.h"

#ifdef __cplusplus
#include "dActor_c.h"

struct CameraTag : dActor_c {
    u8 unk_0d0[0x4];                  /* 0x0d0 */

    virtual ~CameraTag();             /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();     /* slot  0 */
    virtual s32  CleanupResources();  /* slot  3 */
    virtual s32  Behavior();          /* slot  6 */
    virtual s32  Render();            /* slot  9 */
    virtual void OnPendingDestroy();  /* slot 12 */
};

typedef char CameraTag_size_must_be_0xd4[sizeof(CameraTag) == 0xd4 ? 1 : -1];

#else

/* Flat layout for the C translation units, which can express neither the
   base class nor the virtual functions. */
struct CameraTag {
    u8  pad_000[0xd4];
};

#endif /* __cplusplus */

#endif
