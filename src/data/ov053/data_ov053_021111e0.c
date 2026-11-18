/* ov053 cartridge data at 0x021111e0, 12 bytes. */
#include "types.h"

extern char data_ov053_02112394[];

const u32 data_ov053_021111e0[3] = {
    0x0000ffff,
    (u32)&data_ov053_02112394,
    0x00000001,
};
