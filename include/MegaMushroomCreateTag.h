#ifndef MEGAMUSHROOMCREATETAG_H
#define MEGAMUSHROOMCREATETAG_H

#include "dActor_c.h"
#include "dCcAc_c.h"

/* The cartridge calls this class daObjKinokoTag_c in RTTI. The readable
 * MegaMushroomCreateTag spelling is already carried by its five configured
 * virtual symbols and one of its two factories. Both names resolve to the
 * same vtable at ov002 0x02108cf4.
 *
 * The __si_class_type_info record at 0x02108c94 points directly at dActor_c
 * at offset zero. The 31-slot table has exactly the same extent as that base
 * and overrides only InitResources (0), CleanupResources (3), Behavior (6),
 * and the D1/D0 destructor pair (16/17).
 *
 * Both factories allocate 0x110 bytes, construct dActor_c, install this
 * vtable, and construct dCcAc_c at 0x0d4. Both destructor variants destroy
 * that member before chaining to dActor_c, independently proving ownership.
 * Behavior reads all five bytes after the collider; their meanings remain
 * uncertain, so their placeholder names deliberately claim only offsets and
 * widths.
 *
 * The recovered original TU is ov002 [0x020b46a0, 0x020b4a70): both factory
 * functions belong to it. Its data contribution is likewise contiguous:
 * internal RTTI/name, MegaMushroomTag SpawnInfo, MegaMushroomCreateTag
 * SpawnInfo, and the complete 31-slot vtable, ending at the next class RTTI. */
struct MegaMushroomCreateTag : dActor_c {
    u8       pad_0d0[0x4];
    dCcAc_c  mMovingCylinderClsn; /* 0x0d4 */
    u8       unk_108;             /* 0x0108 */
    u8       unk_109;             /* 0x0109 */
    u8       unk_10a;             /* 0x010a */
    u8       unk_10b;             /* 0x010b */
    u8       unk_10c;             /* 0x010c */
    u8       pad_10d[0x3];

    virtual ~MegaMushroomCreateTag(); /* slots 16, 17 */

    virtual s32 InitResources();      /* slot 0 */
    virtual s32 CleanupResources();   /* slot 3 */
    virtual s32 Behavior();           /* slot 6 */

    /* These two names are semantic readings, not cartridge strings. Their
     * ownership is stronger than their spelling: every ROM call passes this
     * class in r0, the only outside caller is Behavior, and the smaller helper
     * tail-calls the larger one after reading this collider. */
    void SpawnMegaMushroom();
    void TrySpawnMegaMushroom();
};

typedef char MegaMushroomCreateTag_size_must_be_0x110[
    sizeof(MegaMushroomCreateTag) == 0x110 ? 1 : -1];

#endif
