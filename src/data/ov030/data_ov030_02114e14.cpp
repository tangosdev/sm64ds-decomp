//cpp
/* .data data at 0x02114e14; bytes from the cartridge. */
#include "types.h"

extern unsigned int data_ov030_02114df8 [];  /* local extern: linker-name reference for a data word */
extern unsigned int data_ov030_02114e90 [];  /* local extern: linker-name reference for a data word */

unsigned int data_ov030_02114e14[3] = {
    (u32)data_ov030_02114df8, (u32)data_ov030_02114e90, 0x0dac0000
};
