#ifndef DAOBJWCOBJ01_C_H
#define DAOBJWCOBJ01_C_H

#include "types.h"

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
    int InitResources(); /* slot  0 */

    virtual ~daObjWcObj01_c() {}
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjWcObj01_c_size_must_be_0x348[sizeof(daObjWcObj01_c) == 0x348 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJWCOBJ01_C_H */
