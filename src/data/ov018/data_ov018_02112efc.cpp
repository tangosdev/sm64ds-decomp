//cpp
/* .data data at 0x02112efc; bytes from the cartridge. */
#include "types.h"

extern unsigned int data_ov018_02112a78[]; /* local extern: linker-name reference for a data word */
extern unsigned int data_ov018_02112a74[]; /* local extern: linker-name reference for a data word */
extern unsigned int data_ov018_02112a98[]; /* local extern: linker-name reference for a data word */
extern unsigned int data_ov018_02112a7c[]; /* local extern: linker-name reference for a data word */

unsigned int data_ov018_02112efc[6] = {
    0x0000005b, (u32)data_ov018_02112a78, (u32)data_ov018_02112a74, (u32)data_ov018_02112a98,
    0x00000001, (u32)data_ov018_02112a7c};
