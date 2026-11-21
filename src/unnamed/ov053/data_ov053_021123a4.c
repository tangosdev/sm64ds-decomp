#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

extern u32 data_ov053_0211239c[];
extern u32 data_ov053_0211241c[];

u32 data_ov053_021123bc[3] = {
    0x5f74616d,
    0x5f657965,
    0x00000072,
};

u32 data_ov053_021123b0[3] = {
    0x5f74616d,
    0x5f657965,
    0x0000006c,
};

u32 data_ov053_021123a4[3] = {
    0x5f74616d,
    0x65636166,
    0x00000000,
};

u32 data_ov053_021123c8[3] = {
    (u32)&data_ov053_0211239c,
    (u32)&data_ov053_0211241c,
    0x06400000,
};
