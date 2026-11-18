//cpp
/* .data data at 0x02114e00; bytes from the cartridge. */
#include "types.h"

extern unsigned int data_ov030_02114fac[]; /* local extern: linker-name reference for a data word */

unsigned int data_ov030_02114e00[2] = {0x00000009, (u32)data_ov030_02114fac};
