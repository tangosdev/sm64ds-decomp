#ifndef DAOBJWCOBJ06_C_H
#define DAOBJWCOBJ06_C_H

#include "types.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Wet-Dry World's long floating board.
 *
 * A daObjFloatBoard_c leaf: no fields of its own. InitResources loads
 * this stage's files, then drops the board onto the water (or the first
 * hit under it).
 */

#ifdef __cplusplus

#include "daObjFloatBoard_c.h"

struct daObjWcObj06_c : daObjFloatBoard_c {
    int InitResources();               /* slot  0 */

    static void *operator new(unsigned long size);
    virtual ~daObjWcObj06_c() {}
};

inline void *daObjWcObj06_c::operator new(unsigned long size)
{
    return _ZN7fBase_cnwEj(size);
}

typedef char daObjWcObj06_c_size_must_be_0x348[sizeof(daObjWcObj06_c) == 0x348 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJWCOBJ06_C_H */
