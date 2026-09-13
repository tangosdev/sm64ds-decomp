#ifndef DAOBJKINOKOTAG_C_H
#define DAOBJKINOKOTAG_C_H

#include "dActor_c.h"
#include "dCcAc_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* daObjKinokoTag_c is the cartridge's own class name: the literal
 * `16daObjKinokoTag_c` is stored in its RTTI type-name object at ov002
 * 0x02108ca0. overlay_actors.md maps MEGA_MUSHROOM_CREATE_TAG(319) and
 * MEGA_MUSHROOM_TAG(320); the ROM debug table names those profiles
 * KINOKO_CREATE_TAG and KINOKO_TAG. MegaMushroomTag and
 * MegaMushroomCreateTag remain useful descriptive names for the two actor
 * factories, but they do not override the ROM-attested C++ class identity.
 *
 * The __si_class_type_info record at 0x02108c94 points directly at dActor_c
 * at offset zero. SIZE 0x110 is the literal both factories pass to
 * operator new. Both construct dCcAc_c at 0x0d4; D1 tears it down before
 * ~dActor_c. dActor_c ends at 0x0d0; pad_0d0 is the 4-byte gap before
 * mMovingCylinderClsn. 0xd4 + 0x34 = 0x108, and 0x108..0x110 is this
 * class's POD. The 3-byte alignment after mLinkedPileGone is implicit.
 *
 * The 31-slot table has the same extent as dActor_c and overrides only
 * InitResources (0), CleanupResources (3), Behavior (6), and the D1/D0
 * pair (16/17). Behavior establishes the five bytes after the collider.
 * The backlink consumer at ov091 0x02133498 is the independent witness
 * for mLinkedPileGone: PILE (Stump, actor 0x1b) writes that byte when its
 * linked actor is KINOKO_TAG (0x140), before the tag respawns the mushroom.
 *
 * Both factories belong to this TU. A single daObjKinokoTag_c_classInit
 * spelling would collide, so the profile-qualified C ABI aliases remain. */

struct daObjKinokoTag_c : dActor_c {
    u8       pad_0d0[0x4];
    dCcAc_c  mMovingCylinderClsn; /* 0x0d4 */
    u8       mHasLinkedPile;       /* 0x0108 */
    u8       mGroupId;             /* 0x0109 */
    u8       mHasMatchingTag;      /* 0x010a */
    u8       mSearchedForPile;     /* 0x010b */
    u8       mLinkedPileGone;      /* 0x010c */

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

    /* Leaf operator new until #2570 puts the same allocator on fBase_c.
       Parameter is unsigned long (size_t on this ABI). `return new`
       relocates to `_Znwm` without this. */
    static void *operator new(unsigned long size)
    {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjKinokoTag_c_size_must_be_0x110[
    sizeof(daObjKinokoTag_c) == 0x110 ? 1 : -1];

#endif
