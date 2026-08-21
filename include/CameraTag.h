/* Hand-written from matched-function evidence:
 * class CameraTag, ov002 0x020b0710-0x020b07c8 (8 functions, no other class
 * in the TU -- tu_map.py).
 *
 * ALL FIVE of its virtual overrides are STUBS -- InitResources, Behavior,
 * Render and CleanupResources are `return 1`, OnPendingDestroy is empty, and
 * not one of them reads `this`. The class exists to BE somewhere: it is a
 * dActor_c and nothing more, marking a position other code queries.
 * Overriding every hook to do nothing is how it stays inert while still being
 * a full actor.
 *
 * SIZE 0xd4, which is sizeof(dActor_c) exactly: CameraTag adds no members at
 * all. It comes from the class's OWN factory, and CameraTag_Spawn spells it
 * `sizeof(struct CameraTag)` so the byte gate holds this header to it:
 *
 *   CameraTag_Spawn  0x020b07c8  new(212 == 0xd4), dActor_c::dActor_c(),
 *                                stores _ZTV9CameraTag. No member ctor runs,
 *                                which is what "no members" looks like.
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
 * Field NAMES are placeholders - renaming cannot change codegen.
 */
#ifndef CAMERATAG_H
#define CAMERATAG_H
#include "types.h"

struct CameraTag {
    u8  pad_000[0xd4];
#ifdef __cplusplus
    /* methods -- every one a stub; see the note above */
    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
#endif
};

typedef char CameraTag_size_must_be_0xd4[sizeof(struct CameraTag) == 0xd4 ? 1 : -1];

#endif
