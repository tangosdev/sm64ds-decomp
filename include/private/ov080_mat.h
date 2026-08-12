#ifndef OV080_MAT_H
#define OV080_MAT_H

#include "types.h"

/* GX material block at *(self+0x1a8) for func_ov080_02125460.
   The union wrapper on `param` is load-bearing for mwccarm 2004/b56
   TEXIMAGE_PARAM coloring (plain `u32 param` misses the r2/r3 rank). */
struct Ov080Mat {
    u32 texAddr;
    u32 pltAddr;
    u32 difAmb;
    u32 speEmi;
    union {
        u32 w;
    } param;
};

#endif
