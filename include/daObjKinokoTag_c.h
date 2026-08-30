#ifndef DAOBJKINOKOTAG_C_H
#define DAOBJKINOKOTAG_C_H

#include "dActor_c.h"
#include "dCcAc_c.h"

/* daObjKinokoTag_c is the cartridge's own class name: the literal
 * `16daObjKinokoTag_c` is stored in its RTTI type-name object at ov002
 * 0x02108ca0. MegaMushroomTag and MegaMushroomCreateTag remain useful
 * descriptive names for the two actor factories, but they do not override
 * the ROM-attested C++ class identity.
 *
 * The __si_class_type_info record at 0x02108c94 points directly at dActor_c
 * at offset zero. The 31-slot table has exactly the same extent as that base
 * and overrides only InitResources (0), CleanupResources (3), Behavior (6),
 * and the D1/D0 destructor pair (16/17).
 *
 * Both factories allocate 0x110 bytes, construct dActor_c, install this
 * vtable, and construct dCcAc_c at 0x0d4. Both destructor variants destroy
 * that member before chaining to dActor_c, independently proving ownership.
 * Behavior establishes the meanings of all five bytes after the collider.
 * The backlink consumer at ov091 0x02133498 provides the independent witness
 * for mLinkedMushroomGone: when its linked actor is this create-tag kind
 * (actor ID 0x140), it writes that byte before the tag respawns the mushroom.
 *
 * The recovered original TU is ov002 [0x020b46a0, 0x020b4a70): both factory
 * functions belong to it. Its data contribution is likewise contiguous:
 * class RTTI/name, MegaMushroomTag SpawnInfo, MegaMushroomCreateTag
 * SpawnInfo, and the complete 31-slot vtable, ending at the next class RTTI. */
struct daObjKinokoTag_c : dActor_c {
    u8       pad_0d0[0x4];
    dCcAc_c  mMovingCylinderClsn; /* 0x0d4 */
    u8       mHasLinkedMushroom;   /* 0x0108 */
    u8       mGroupId;             /* 0x0109 */
    u8       mHasMatchingTag;      /* 0x010a */
    u8       mSearchedForMushroom; /* 0x010b */
    u8       mLinkedMushroomGone;  /* 0x010c */
    u8       pad_10d[0x3];

    /* InitResources is the first out-of-line virtual/key function. Together
     * with this inline destructor, mwccarm owns the retail D1/D0 pair and the
     * complete class RTTI/vtable group without retaining a D2 body. */
    virtual ~daObjKinokoTag_c() {}    /* slots 16, 17 */

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

typedef char daObjKinokoTag_c_size_must_be_0x110[
    sizeof(daObjKinokoTag_c) == 0x110 ? 1 : -1];

/* POD view used only to preserve the three-word Vector3 call ABI without
 * emitting Vector3's vague-linkage destructor. */
struct KinokoPositionWords {
    Fix12i x;
    Fix12i y;
    Fix12i z;
};

typedef char KinokoPositionWords_size_must_be_0xc[
    sizeof(KinokoPositionWords) == 0xc ? 1 : -1];

/* Typed owner for the two 0x1c actor descriptors at ov002:0x02108cb4 and
 * 0x02108cd0. Their English global spellings remain evidence-bounded ABI
 * aliases; the structure and factory relationships are ROM-proven. */
struct KinokoTagSpawnInfo {
    daObjKinokoTag_c *(*spawn)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 flags;
    Fix12i rangeOffsetY;
    Fix12i range;
    Fix12i drawDistance;
    u32 unk_18;
};

typedef char KinokoTagSpawnInfo_size_must_be_0x1c[
    sizeof(KinokoTagSpawnInfo) == 0x1c ? 1 : -1];

#endif
