/* ov053 cartridge data at 0x02111384, 40 bytes. */
#include "types.h"

extern char data_ov053_021123a4[];

const u32 data_ov053_02111384[10] = {
    0x0000ffff, (u32)&data_ov053_021123a4,
    0x00000101, 0x005a0101,
    0x00b40101, 0x010e0101,
    0x01680101, 0x01c20101,
    0x00b00001, 0x00b00001,
};
