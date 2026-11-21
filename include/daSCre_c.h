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

    virtual ~daSCre_c() {}
    virtual s32 Behavior();
};

typedef char daSCre_c_size_must_be_0xd4[sizeof(daSCre_c) == 0xd4 ? 1 : -1];

#endif /* DASCRE_C_H */
