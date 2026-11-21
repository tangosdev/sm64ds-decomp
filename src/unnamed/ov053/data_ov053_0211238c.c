#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

extern u32 data_ov053_021123d4[];
extern u32 data_ov053_0211238c[];

u32 data_ov053_02112394[2] = {
    0x5f736167,
    0x0074616d,
};

u32 data_ov053_0211238c[2] = {
    0x00000100,
    (u32)&data_ov053_021123d4,
};

u32 data_ov053_0211239c[2] = {
    0x00000001,
    (u32)&data_ov053_0211238c,
};
