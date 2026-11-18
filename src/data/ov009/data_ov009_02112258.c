/* ov009 cartridge data at 0x02112258, 24 bytes. */
#include "types.h"

extern char data_ov009_02112b7c[];

const u32 data_ov009_02112258[6] = {
    (u32)&data_ov009_02112b7c, 0x00000001, 0x00000001, 0x00000001, 0x0000001f, 0x0000001f,
};
