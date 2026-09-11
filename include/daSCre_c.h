#ifndef DASCRE_C_H
#define DASCRE_C_H

#include "types.h"
#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Star-create trigger. One shot: spawn the star when the closest player
 * is strictly within 100 units, then mark this trigger for removal
 * regardless.
 */
struct daSCre_c : dActor_c {
    u8 unk_0d0[0x4];       /* 0x0d0 */

    virtual ~daSCre_c() {}
    virtual s32 Behavior();

    /* The compiler requires unsigned long here; the actor allocator uses
       the configured unsigned-int nwEj entry. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }

    /* Local view of the inherited scalar position for Spawn.
       The shared position layout remains unresolved. */
    const Vector3 &Pos() const {
        return *reinterpret_cast<const Vector3 *>(&mPosX);
    }
};

typedef char daSCre_c_size_must_be_0xd4[sizeof(daSCre_c) == 0xd4 ? 1 : -1];

#endif /* DASCRE_C_H */
