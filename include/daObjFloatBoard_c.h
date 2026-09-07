#ifndef DAOBJFLOATBOARD_C_H
#define DAOBJFLOATBOARD_C_H

#include "types.h"
#include "dBgActor_c.h"

/* The abstract base of the planks that float on water and tip when you stand on
 * them: Jolly Roger Bay's raft and the two Wet-Dry World boards.
 *
 * A LAYER THE TREE DID NOT HAVE. The ROM's RTTI names this class and points it at
 * dBgActor_c; the tree named the base's base for all three leaves.
 *
 *   _ZTI17daObjFloatBoard_c  ov002 0x02108fb4
 *   _ZTS17daObjFloatBoard_c  ov002 0x02108fc0   "17daObjFloatBoard_c"
 *   vtable                   ov002 0x02108fdc, 32 slots, same count as the base
 *   kind                     __si_class_type_info, ONE base, subobject offset 0
 *   base                     dBgActor_c, ov002 0x021089ec -- the tree's dBgActor_c
 *
 * ABSTRACT IN ONE SLOT ONLY, which makes it the odd one out in this series. Slot 0
 * (InitResources) is null and each leaf supplies it, but slot 3
 * (CleanupResources) is NOT: this class supplies it at `func_ov002_020b5be0`, and
 * all three leaves inherit that word. Its other own overrides are slots 6
 * (Behavior), 9 (Render), 16 (D1) and 17 (D0).
 *
 * THREE DESCENDANTS: daObjKi_Ita_c (coined FloatOnWaterPlatformJrb),
 * daObjWcObj01_c (coined FloatOnWaterPlatformWdwSquare) and daObjWcObj06_c
 * (coined FloatOnWaterPlatformWdwRectangle). Each destructor stores this
 * class's vtable between its own and _ZTV10dBgActor_c.
 *
 * SIZE 0x348, the literal all three factories pass to fBase_c::operator new.
 *
 * THE FIELDS BELOW ARE THIS CLASS'S, read off methods that are this class's own
 * vtable slots and live in this class's own overlay. A method can reach its own
 * members and its ancestors', never a descendant's.
 *
 *   0x324  the water surface height. `func_ov002_020b5ab4`, the helper this
 *          class's Behavior calls to ask whether the board is on water, writes it
 *          from the raycast result, and Behavior then sets the actor's Y from it.
 *   0x32c  the file table. This class's own CleanupResources releases entries [0]
 *          and [1] through it.
 *   0x330  how far the board has sunk, walked toward -0x28000 while a rider is
 *          present and back to 0 when there is not.
 *   0x334  the bob offset, 10 * sin(phase).
 *   0x338  the bob phase, stepped 0xa00 a frame.
 *   0x33c  the rider. Behavior reads the actor's position through it and tilts
 *          the board toward the horizontal bearing.
 *   0x342  the rider timeout; when it reaches zero 0x33c is cleared.
 *   0x344  a stage-specific fallback water level, read only when the current
 *          stage id is 0x15.
 *
 * 0x344 + 4 = 0x348 closes the class on the factories' literal. The four gaps --
 * 0x320, 0x328, 0x33a and 0x340 -- are UNOBSERVED, not inherited: nothing this
 * class's own methods touch lands in them and no leaf declares anything there.
 * Field NAMES are coined from what the code does; nothing in the ROM names them.
 */

#ifdef __cplusplus

struct daObjFloatBoard_c : dBgActor_c {
    /* Field NAMES are placeholders. Offsets, widths and types are observed. */
    u8   pad_320[0x4];
    s32  mWaterY;           /* 0x324 */
    u8   pad_328[0x4];
    void **mFileTable;      /* 0x32c */
    s32  mSinkOffset;       /* 0x330 */
    s32  mBobOffset;        /* 0x334 */
    s16  mBobPhase;         /* 0x338 */
    u8   pad_33a[0x2];
    void *mRider;           /* 0x33c */
    u8   pad_340[0x2];
    u16  mRiderTimeout;     /* 0x342 */
    s32  mFallbackWaterY;   /* 0x344 */

    /* --- vtable --- */
    /* INLINE ON PURPOSE -- see include/dBgActor_c.h. The out-of-line copy is at
       ov002 0x020b5a70, still under its func_ov002_ name. */
    virtual ~daObjFloatBoard_c() {}

    int CleanupResources();             /* slot 3, ov002 0x020b5be0 */
    int Behavior();                     /* slot 6, ov002 0x020b5c4c */
    s32 Render();                       /* slot 9, ov002 0x020b5c24 -- mModel.Render(0) */
    /* THE NULL SLOTS THE NOTE ABOVE ALREADY NAMES, SPELT SO THE COMPILER AGREES.
       mwccarm lays down a bare 0x00000000 with no relocation for a pure virtual --
       there is no __cxa_pure_virtual in this image for it to point at -- so a zero
       word in a ROM vtable IS the `= 0`, and it is the only thing that produces one.
       Left undeclared, this class silently inherits dBgActor_c's concrete bodies and
       the vtable it emits disagrees with the cartridge at exactly these slots.
       Measured by tools/romdata_check.py, which is the only gate that reads them:
       the ROM build's 106/106 compares .text alone and is blind here.

       DECLARED LAST, AND WITH `virtual` -- unlike the plain overrides above. The
       pure-specifier is only valid on a declaration carrying the keyword, and a
       pure virtual has no body to emit, so it can never become the key function:
       whichever virtual was first and non-inline before is still first now. */
    virtual int InitResources() = 0;        /* slot  0 */
};

typedef char daObjFloatBoard_c_size_must_be_0x348[sizeof(daObjFloatBoard_c) == 0x348 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJFLOATBOARD_C_H */
