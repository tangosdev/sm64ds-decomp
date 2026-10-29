#ifndef DASCRE_C_H
#define DASCRE_C_H

#include "types.h"
#include "dActor_c.h"

/**
 * Star-create trigger. One shot: spawn the star when the closest player
 * is strictly within 100 units, then mark this trigger for removal
 * regardless.
 */
struct daSCre_c : dActor_c {
    u8 pad_0d0[0x4];       /* 0x0d0 unused */

    virtual ~daSCre_c() {}
    virtual s32 Behavior();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daSCre_c_size_must_be_0xd4[sizeof(daSCre_c) == 0xd4 ? 1 : -1];
#endif

#endif /* DASCRE_C_H */
