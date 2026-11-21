#include "types.h"

extern u32 data_ov009_02112c58[];
extern u32 data_ov009_02112b94[];

u32 data_ov009_02112c9c[3] = {
    0x00000400,
    (u32)&data_ov009_02112c58[43],
    0x00001d05,
};

u32 data_ov009_02112ca8[8] = {
    (u32)&data_ov009_02112c58[241], 0x00001c20, (u32)&data_ov009_02112c58[367], 0x00000325,
    (u32)&data_ov009_02112b94,      0x00001140, (u32)&data_ov009_02112c58[299], 0x00000845,
};
