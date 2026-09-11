#ifndef DAOBJFL_FALL_BLOCK_C_H
#define DAOBJFL_FALL_BLOCK_C_H

#include "types.h"
#include "daObjFallBlock_c.h"

/* Lethal Lava Land's falling block. */

#ifdef __cplusplus

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daObjFl_Fall_Block_c : daObjFallBlock_c {
    static void *operator new(unsigned long size);
    virtual ~daObjFl_Fall_Block_c() {}
    int CleanupResources();
    int InitResources();
};

inline void *daObjFl_Fall_Block_c::operator new(unsigned long size)
{
    return _ZN7fBase_cnwEj(size);
}

typedef char daObjFl_Fall_Block_c_size_must_be_0x34c[sizeof(daObjFl_Fall_Block_c) == 0x34c ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJFL_FALL_BLOCK_C_H */
