#ifndef DAOBJFL_KOMAU_C_H
#define DAOBJFL_KOMAU_C_H

#include "types.h"

/* daObjFl_KomaU_c: the rotating bar of flames (registry profile FL_KOMA_U).
 * Derives from dBgActor_c, and the ONE thing it adds beyond two particle-id
 * arrays is the array of collision cylinders -- one per flame.
 *
 *     dCcPos_c[8]   0x360 + 8 * 0x3c = 0x540
 *
 * The ROM destroys it with
 * __cxa_vec_cleanup(this + 0x360, 8, 0x3c, _ZN8dCcPos_cD1Ev): 0x3c is
 * sizeof(dCcPos_c) and the element destructor is a name the tree
 * already had. The destructor also calls _ZN10dBgW_KcMbgD1Ev at +0x124 and
 * _ZN5ModelD1Ev at +0xd4 (the inherited mMeshCollider and mModel), D1 and not
 * D2, so they are members and not inlined bases.
 *
 * BASE: dBgActor_c, direct. The __si_class_type_info record _ZTI15daObjFl_KomaU_c
 * at ov064 0x0211bdcc points at _ZTS15daObjFl_KomaU_c (0x0211bdd8) and at
 * _ZTI10dBgActor_c, and the vtable at 0x0211be10 carries that _ZTI at its
 * address point minus 4. The tree previously called the class RotatingFirebar
 * (coined). SIZE 0x540 is the literal the factory daObjFl_KomaU_c_classInit
 * passes to fBase_c::operator new.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "dCcPos_c.h"

struct daObjFl_KomaU_c : dBgActor_c {
    /* dBgActor_c ends at 0x31e; these ids are 4-byte aligned. */
    u8 pad_31e[2];
    u32 mParticleID[2][8]; /* 0x320, 0x340 */
    dCcPos_c mColliders[8]; /* 0x360 */

    /* Out of line in daObjFl_KomaU_c.cpp: the key function, so that TU emits
     * _ZTV/_ZTI/_ZTS15daObjFl_KomaU_c. */
    virtual ~daObjFl_KomaU_c();

    /* Overrides of fBase_c's slots 0, 3, 6 and 9. */
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjFl_KomaU_c_size_must_be_0x540[sizeof(daObjFl_KomaU_c) == 0x540 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJFL_KOMAU_C_H */
