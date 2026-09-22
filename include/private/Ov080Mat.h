#ifndef OV080_MAT_H
#define OV080_MAT_H

#include "types.h"

/* GX material block at *(self+0x1a8) for func_ov080_02125460. */
struct Ov080Mat {
    u32 texAddr; /* 0x00 */
    u32 pltAddr; /* 0x04 */
    u32 difAmb;  /* 0x08 */
    u32 speEmi;  /* 0x0c */
    u32 param;   /* 0x10 */
};

#endif
