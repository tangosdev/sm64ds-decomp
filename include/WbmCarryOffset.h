#ifndef WBM_CARRY_OFFSET_H
#define WBM_CARRY_OFFSET_H

#include "nitro/fx/fx.h"

/* POD triple. Vector3's constructor would move the table out of .rodata. */
struct WbmCarryOffset {
    fx32 x;
    fx32 y;
    fx32 z;
};

#endif
