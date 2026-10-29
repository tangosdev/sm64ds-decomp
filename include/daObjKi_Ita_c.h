#ifndef DAOBJKI_ITA_C_H
#define DAOBJKI_ITA_C_H

#include "types.h"

/**
 * Jolly Roger Bay floating plank (`ita` = board).
 *
 * A daObjFloatBoard_c leaf: no fields of its own. InitResources hands
 * this overlay's files to the shared ov002 helper; CleanupResources
 * is the base's.
 */

#ifdef __cplusplus

#include "daObjFloatBoard_c.h"

struct daObjKi_Ita_c : daObjFloatBoard_c {
    int InitResources(); /* slot  0 */

    virtual ~daObjKi_Ita_c() {}
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKi_Ita_c_size_must_be_0x348[sizeof(daObjKi_Ita_c) == 0x348 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJKI_ITA_C_H */
