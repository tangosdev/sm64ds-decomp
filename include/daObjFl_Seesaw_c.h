#ifndef DAOBJFL_SEESAW_C_H
#define DAOBJFL_SEESAW_C_H

#include "types.h"
#include "dBgActor_c.h"

/* daObjFl_Seesaw_c is proven by _ZTS at ov022 0x02113ffc, the factory's
 * allocation size (0x324) and a vtable that only redeclares the slots
 * below. mSwingStep sits at 0x31e, in dBgActor_c's tail padding. */
struct daObjFl_Seesaw_c : dBgActor_c {
    s16 mSwingStep;          /* 0x31e */
    u8  mSwingCooldown;      /* 0x320 */
    u8  pad_321[0x3];

    virtual ~daObjFl_Seesaw_c();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */

    void func_ov022_02111d48();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjFl_Seesaw_c_size_must_be_0x324[sizeof(daObjFl_Seesaw_c) == 0x324 ? 1 : -1];
#endif

#endif /* DAOBJFL_SEESAW_C_H */
