//cpp
/* .data data at 0x02112eb4; bytes from the cartridge. */
#include "types.h"

extern unsigned int data_ov018_02112ea0[]; /* local extern: linker-name reference for a data word */
extern unsigned int data_ov018_02112efc[]; /* local extern: linker-name reference for a data word */

unsigned int data_ov018_02112eb4[3] = {(u32)data_ov018_02112ea0, (u32)data_ov018_02112efc,
                                       0x157c0000};
