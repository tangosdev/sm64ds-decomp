#ifndef DAOBJKM3_KAITENDAI_C_H
#define DAOBJKM3_KAITENDAI_C_H

#include "types.h"
#include "daObjKaitendai_c.h"

/**
 * daKpa_c in the Sky's spinning disc. No fields of its own:
 * sizeof(daObjKm3_Kaitendai_c) == 0x320 == sizeof(dBgActor_c).
 * Overrides the two slots the base leaves null (InitResources,
 * CleanupResources) and hands this overlay's model/collision
 * descriptor to the shared ov002 helpers. The default per-frame rotation
 * increment is one of two statics, chosen by param1's low byte. Shared setup
 * can override it with a nonzero spawn mAngleZ.
 *
 * `daObjKm3_Kaitendai_c` is the RTTI name.
 */
struct daObjKm3_Kaitendai_c : daObjKaitendai_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjKm3_Kaitendai_c() {}
    virtual s32 CleanupResources(); /* slot 3 */
    virtual s32 InitResources();    /* slot 0 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char
    daObjKm3_Kaitendai_c_size_must_be_0x320[sizeof(daObjKm3_Kaitendai_c) == 0x320 ? 1 : -1];
#endif

#endif /* DAOBJKM3_KAITENDAI_C_H */
