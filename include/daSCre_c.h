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
    u8 pad_0d0[0x4];       /* 0x0d0 unused */

    virtual ~daSCre_c() {}
    virtual s32 Behavior();

    /* In-class operator new is unsigned long (C++). The actor-heap
       allocator is _ZN7fBase_cnwEj (unsigned int). The cast is that
       bridge; the factory stays `return new daSCre_c`. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    /* x/y/z sit in a row on dActor_c; Spawn wants a Vector3. */
    const Vector3 &Pos() const {
        return *reinterpret_cast<const Vector3 *>(&mPosX);
    }
};

typedef char daSCre_c_size_must_be_0xd4[sizeof(daSCre_c) == 0xd4 ? 1 : -1];

#endif /* DASCRE_C_H */
