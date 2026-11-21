#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

extern u32 data_ov026_02112ee0[];
extern u32 data_ov026_02113014[];
extern u32 data_ov026_02112f94[];

u32 data_ov026_02112dd8[2] = {
    0x00000003,
    (u32)data_ov026_02112ee0,
};

u32 data_ov026_02112dd0[2] = {
    0x6d757a75,
    0x00696b61,
};

u32 data_ov026_02112dc8[2] = {
    0x00000009,
    (u32)data_ov026_02113014,
};

u32 data_ov026_02112de0[2] = {
    0x00000008,
    (u32)data_ov026_02112f94,
};
