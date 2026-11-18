//cpp
/* .rodata data at 0x021118bc; bytes from the cartridge. */
#include "types.h"

extern unsigned int data_ov024_021128b4[]; /* local extern: linker-name reference for a data word */
extern unsigned int data_ov024_021128cc[]; /* local extern: linker-name reference for a data word */

extern unsigned int const data_ov024_021118bc[14] = {0x0000ffff,
                                                     (u32)data_ov024_021128b4,
                                                     0x00000001,
                                                     0x00010001,
                                                     0x00000001,
                                                     0x00000028,
                                                     0x00000001,
                                                     0x0000ffff,
                                                     (u32)data_ov024_021128cc,
                                                     0x00000001,
                                                     0x00010001,
                                                     0x00000001,
                                                     0x00000028,
                                                     0x00000001};
