#ifndef DAOBJFL_LONDON_C_H
#define DAOBJFL_LONDON_C_H

#include "types.h"
#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Tilting bridge in Lethal Lava Land. */
struct daObjFl_London_c : dBgActor_c {
    static void *operator new(unsigned long size);
    virtual ~daObjFl_London_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();

    u8 mCooldown;                         /* 0x31e */
    u8 mFlag;                             /* 0x31f */
};

inline void *daObjFl_London_c::operator new(unsigned long size)
{
    return _ZN7fBase_cnwEj(size);
}

typedef char daObjFl_London_c_size_must_be_0x320[
    sizeof(daObjFl_London_c) == 0x320 ? 1 : -1];

#endif /* DAOBJFL_LONDON_C_H */
