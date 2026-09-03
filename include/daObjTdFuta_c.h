#ifndef DAOBJTDFUTA_C_H
#define DAOBJTDFUTA_C_H

#include "types.h"
#include "dBgActor_c.h"

/* The cartridge RTTI names this class daObjTdFuta_c. daObjTdFuta_c is the
 * spelling now carried by all six named virtuals and the factory; the coined
 * HugeCover alias that shared 0x021138e0 in symbols.txt was deleted. The __si_class_type_info record points directly at dBgActor_c.
 * Its table at 0x021138e0 has the same 32-slot extent as that base; slots 0,
 * 3, 6, 9, 16 and 17 are the only overrides.
 *
 * daObjTdFuta_c adds no storage. The factory allocates exactly 0x320 bytes, calls
 * dBgActor_c's constructor, and installs this table. The empty derived
 * destructor correspondingly lets the compiler destroy the inherited
 * dBgActor_c object. The adjacent 0x021139a4 table is a different class:
 * RTTI names it daObjTdWater_c and its slots point into HugeWater's TU.
 */
struct daObjTdFuta_c : dBgActor_c {

    /* MEASURED -- INLINE ON PURPOSE. The class TU is the only place these
       two are emitted; with the body out of line mwcc emits D0 ahead of D1
       and the ROM has D1 first (rombuild refuses the object outright). An
       inline body also drops the D2 variant the cartridge never carried. */
    virtual ~daObjTdFuta_c() {}            /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();           /* slot  0 */
    virtual s32 CleanupResources();        /* slot  3 */
    virtual s32 Behavior();                /* slot  6 */
    virtual s32 Render();                  /* slot  9 */
};

typedef char daObjTdFuta_c_size_must_be_0x320[sizeof(daObjTdFuta_c) == 0x320 ? 1 : -1];

#endif /* DAOBJTDFUTA_C_H */
