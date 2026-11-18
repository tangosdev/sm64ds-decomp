//cpp
/* .rodata data at 0x02111be0; bytes from the cartridge. */
#include "types.h"

extern unsigned int data_ov035_02112064[]; /* local extern: linker-name reference for a data word */

extern unsigned int const data_ov035_02111be0[7] = {
    0x0000ffff, (u32)data_ov035_02112064, 0x00000001, 0x00000001, 0x00000001, 0x0000005b,
    0x00000001};
