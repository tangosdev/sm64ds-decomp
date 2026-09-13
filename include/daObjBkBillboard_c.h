#ifndef DAOBJBKBILLBOARD_C_H
#define DAOBJBKBILLBOARD_C_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Whomp's Fortress pole billboard (profile BK_BILLBOARD /
 * POLE_BILLBOARD 42). Direct base is dActor_c. Factory allocates
 * 0x124 = sizeof(dActor_c) plus pad_0d0[4] plus Model.
 *
 * `daObjBkBillboard_c` is the RTTI name.
 */
struct daObjBkBillboard_c : dActor_c {
    u8    pad_0d0[0x4];
    Model mModel;            /* 0x0d4 */

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjBkBillboard_c() {}     /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();         /* slot  0 */
    virtual s32 CleanupResources();      /* slot  3 */
    virtual s32 Render();                /* slot  9 */

    /* size_t == unsigned long here; unsigned int is illegal. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjBkBillboard_c_size_must_be_0x124[
    sizeof(daObjBkBillboard_c) == 0x124 ? 1 : -1];

#endif /* DAOBJBKBILLBOARD_C_H */
