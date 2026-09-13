#ifndef DAOBJWCOBJ01_C_H
#define DAOBJWCOBJ01_C_H

#include "types.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Wet-Dry World's square floating board.
 *
 * A daObjFloatBoard_c leaf: no fields of its own. InitResources loads
 * WC_OBJ01's model/KCL/CLPS, then drops the board onto the water (or
 * the first hit under it).
 */

#ifdef __cplusplus

#include "daObjFloatBoard_c.h"

struct daObjWcObj01_c : daObjFloatBoard_c {
    int InitResources();               /* slot  0 */

    static void *operator new(unsigned long size);
    virtual ~daObjWcObj01_c() {}
};

inline void *daObjWcObj01_c::operator new(unsigned long size)
{
    return _ZN7fBase_cnwEj(size);
}

typedef char daObjWcObj01_c_size_must_be_0x348[sizeof(daObjWcObj01_c) == 0x348 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJWCOBJ01_C_H */
