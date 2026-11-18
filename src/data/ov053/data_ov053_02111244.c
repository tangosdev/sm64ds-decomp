/* ov053 cartridge data at 0x02111244, 40 bytes. */
#include "types.h"

extern char data_ov053_021123bc[];

const u32 data_ov053_02111244[10] = {
    0x00000001, 0x00000001, 0x0001002c, 0x0000ffff, (u32)&data_ov053_021123bc,
    0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x002d002c,
};
