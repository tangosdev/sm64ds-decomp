#ifndef DAOBJKI_ITA_C_H
#define DAOBJKI_ITA_C_H

#include "types.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Jolly Roger Bay floating plank (`ita` = board).
 *
 * A daObjFloatBoard_c leaf: no fields of its own. InitResources loads
 * this stage's files; CleanupResources is the base's.
 */

#ifdef __cplusplus

#include "daObjFloatBoard_c.h"

struct daObjKi_Ita_c : daObjFloatBoard_c {
    int InitResources();               /* slot  0 */

    static void *operator new(unsigned long size);
    virtual ~daObjKi_Ita_c() {}
};

inline void *daObjKi_Ita_c::operator new(unsigned long size)
{
    return _ZN7fBase_cnwEj((unsigned)size);
}

typedef char daObjKi_Ita_c_size_must_be_0x348[sizeof(daObjKi_Ita_c) == 0x348 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKI_ITA_C_H */
