/* ov053 cartridge data at 0x02111314, 56 bytes. */
#include "types.h"

extern char data_ov053_021123b0[];
extern char data_ov053_021123bc[];

const u32 data_ov053_02111314[14] = {
    0x0000ffff,
    (u32)&data_ov053_021123b0,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x0001004b,
    0x0000ffff,
    (u32)&data_ov053_021123bc,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x004c004b,
};
