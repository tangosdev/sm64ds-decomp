#ifndef DAOBJFL_LONDON_C_H
#define DAOBJFL_LONDON_C_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjFl_London_c_classInit allocates 0x320 bytes, calls dBgActor_c's
 *   constructor, and installs this class's vptr. The base constructs Model
 *   and dBgW_KcMbg; the destructor destroys those inherited members in reverse
 *   order before the dActor_c base. This class's two bytes need no destruction.
 *
 * SIZE 0x320 is the factory's literal; mFlag at 0x31f closes that layout.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 *
 * mCooldown/mFlag sit at 0x31e/0x31f, in dBgActor_c's tail padding.
 * InitResources seeds them to 0xf and 0. Behavior decrements the cooldown,
 * uses mFlag to choose rotation toward -0x2000 or zero, then reloads the
 * cooldown and flips the flag at each limit. These member names are inferred
 * from those uses, not recovered identifiers.
 *
 * The complete eight-function TU is promoted. Render uses the inherited
 * Model, and the matrix helper uses this class's real members while retaining
 * its existing external char* boundary. Manual factory construction and raw
 * accesses in Behavior/CleanupResources remain partial reconstruction under
 * issue #2480; promotion does not mean those interfaces have been recovered.
 */
struct daObjFl_London_c : dBgActor_c {
    /* The destructor is declared FIRST and defined INLINE: mwcc then emits the
     * D1/D0 pair in retail order and no homeless D2, which is what lets the
     * whole class isolate into one translation unit. */
    virtual ~daObjFl_London_c() {}         /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */

    u8  mCooldown;                         /* 0x31e */
    u8  mFlag;                             /* 0x31f */
};

typedef char daObjFl_London_c_size_must_be_0x320[sizeof(daObjFl_London_c) == 0x320 ? 1 : -1];

#endif /* DAOBJFL_LONDON_C_H */
