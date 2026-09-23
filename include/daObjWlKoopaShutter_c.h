#ifndef DAOBJWLKOOPASHUTTER_C_H
#define DAOBJWLKOOPASHUTTER_C_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjWlKoopaShutter_c_classInit (historically BowserShutter_Spawn)
 *                 fBase_c::operator new(800 = 0x320), dBgActor_c::dBgActor_c(), stores _ZTV21daObjWlKoopaShutter_c,
 *                 then the members below in this order.
 *   ~daObjWlKoopaShutter_c   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x320 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct daObjWlKoopaShutter_c : dBgActor_c {

    virtual ~daObjWlKoopaShutter_c();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjWlKoopaShutter_c_size_must_be_0x320[sizeof(daObjWlKoopaShutter_c) == 0x320 ? 1 : -1];
#endif

#endif /* DAOBJWLKOOPASHUTTER_C_H */
