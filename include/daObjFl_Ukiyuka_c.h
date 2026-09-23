#ifndef DAOBJFL_UKIYUKA_C_H
#define DAOBJFL_UKIYUKA_C_H

#include "types.h"

/* Lethal Lava Land's floating floor -- both sizes of it.
 *
 * Derives from daObjUkiyuka_c (which derives from dBgActor_c); the
 * destructor's three vptr stores prove the two-level chain. One field
 * of its own, mVariant at 0x32c: 0 for spawn ID 0x4b, 1 for 0x4c.
 * Both factories (FL_UKIYUKA_L / FL_UKIYUKA) allocate 0x330 and store
 * this vtable.
 */

#ifdef __cplusplus

#include "daObjUkiyuka_c.h"

struct daObjFl_Ukiyuka_c : daObjUkiyuka_c {
    u8  mVariant;           /* 0x32c */
    /* --- vtable --- */
    virtual ~daObjFl_Ukiyuka_c(); /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjFl_Ukiyuka_c_size_must_be_0x330[sizeof(daObjFl_Ukiyuka_c) == 0x330 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJFL_UKIYUKA_C_H */
