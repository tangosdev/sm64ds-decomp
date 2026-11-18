//cpp
/* .rodata data at 0x02111e68; bytes from the cartridge. */
#include "types.h"

extern unsigned int data_ov045_02112488[]; /* local extern: linker-name reference for a data word */

extern unsigned int const data_ov045_02111e68[7] = {
    0x0000ffff, (u32)data_ov045_02112488, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
    0x000100c8};
