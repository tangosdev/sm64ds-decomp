/* ov009 cartridge data at 0x02112dcc, 72 bytes. */
#include "types.h"

extern u32 data_ov009_02112b60[];
extern u32 data_ov009_02112b64[];
extern u32 data_ov009_02112b68[];
extern u32 data_ov009_02112c78[];
extern u32 data_ov009_02112ccc[];
extern u32 data_ov009_02112d84[];
extern u32 data_ov009_02112e14[];
extern u32 data_ov009_02112f3c[];
extern u32 data_ov009_021133d4[];

u32 data_ov009_02112dcc[18] = {
    0x0000dc02, (u32)&data_ov009_021133d4, 0x00001103, (u32)&data_ov009_02112e14,
    0x00000504, (u32)&data_ov009_02112d84, 0x0000010b, (u32)&data_ov009_02112b64,
    0x00000e01, (u32)&data_ov009_02112f3c, 0x00000309, (u32)&data_ov009_02112c78,
    0x0000010c, (u32)&data_ov009_02112b60, 0x0000010e, (u32)&data_ov009_02112b68,
    0x0000042a, (u32)&data_ov009_02112ccc,
};
