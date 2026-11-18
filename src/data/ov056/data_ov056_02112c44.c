/* ov056 cartridge data at 0x02112c44, 8 bytes. */
#include "types.h"

extern u32 data_ov056_021115d0[];

u32 data_ov056_02112c44[2] = {
    0x00000003,
    (u32)&data_ov056_021115d0,
};
