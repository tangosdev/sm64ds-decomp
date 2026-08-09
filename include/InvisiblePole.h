/* Hand-written from matched-function evidence:
 * class InvisiblePole, ov002 0x020b0710-0x020b07c8 (8 functions, no other class
 * in the TU -- tu_map.py).
 *
 * ALL FIVE of its virtual overrides are STUBS -- InitResources, Behavior,
 * Render and CleanupResources are `return 1`, OnPendingDestroy is empty, and
 * not one of them reads `this`. The class exists to BE somewhere: it is an
 * Actor carrying a MovingCylinderClsn and nothing else, so what it does is
 * occupy a collision volume that other code queries. Overriding every hook to
 * do nothing is how it stays inert while still being a full actor.
 *
 * The layout therefore comes from the spawner and the constructor, not from
 * the methods -- none of them touches a field:
 *
 *   InvisiblePole_Spawn allocates 264 == 0x108 bytes, so that is sizeof.
 *   It then runs MovingCylinderClsn's constructor at +0xd4, and Actor is
 *   0xd0, so the sub-object begins on the first 4-aligned offset past the
 *   base and runs to the end: 0x108 - 0xd4 == 0x34.
 *
 * That 0x34 is corroborated independently -- KoopaShell puts a
 * MovingCylinderClsn at 0x110 and its next member at 0x144, the same span.
 *
 * Field NAMES are placeholders - renaming cannot change codegen.
 */
#ifndef INVISIBLEPOLE_H
#define INVISIBLEPOLE_H
#include "types.h"

struct InvisiblePole {
    u8  pad_000[0xd4];
    /* MovingCylinderClsn, 0x34 bytes, running to the end of the object. Kept
       as a byte marker: nothing in this class ever looks inside it. */
    u8  mCylinderClsn;            /* 0x0d4 */
    u8  pad_0d5[0x33];
#ifdef __cplusplus
    /* methods -- every one a stub; see the note above */
    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
#endif
};

#endif
