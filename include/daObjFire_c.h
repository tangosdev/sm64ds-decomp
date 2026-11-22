#ifndef DAOBJFIRE_C_H
#define DAOBJFIRE_C_H

#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

/* Red and blue flames. RTTI ov002:0x02108ee8 names daObjFire_c;
 * overlay_actors.md lists RED_FLAME(316) and BLUE_FLAME(317). The
 * debug table names those profiles OBJ_RED_FIRE / OBJ_BLUE_FIRE.
 * POPPING_LAVA_BUBBLES(196) is daObjLava_c.
 *
 * SIZE 0x118, the literal both factories pass to operator new.
 * Factory constructs dCcAc_c at 0xe4; D1 tears it down before ~dActor_c.
 * dActor_c ends at 0xd4, where mFlamePos starts.
 * 0xe4 + 0x34 = 0x118, closing on the allocation.
 */
struct daObjFire_c : dActor_c {
    Vector3 mFlamePos;  /* 0x0d4 */
    u8 mDisappearTimer; /* 0x0e0 */
    u8 pad_0e1[0x3];
    dCcAc_c mdCcAc_c; /* 0x0e4 */

    /* Inline and first: out-of-line mwccarm emits D0 before D1; retail
       has D1 at 0x020b5734 below D0 at 0x020b5764. */
    virtual ~daObjFire_c() {}
    virtual int InitResources();                /* slot  0 */
    virtual int Behavior();                     /* slot  6 */
    virtual s32 OnYoshiTryEat();                /* slot 18 */
    virtual void OnTurnIntoEgg(daPly_c &player); /* slot 19 */
};

typedef char daObjFire_c_size_must_be_0x118[sizeof(daObjFire_c) == 0x118 ? 1 : -1];

#endif /* DAOBJFIRE_C_H */
