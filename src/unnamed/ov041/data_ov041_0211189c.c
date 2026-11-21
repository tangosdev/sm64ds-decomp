#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

extern u32 data_ov041_0211188c[];
extern u32 data_ov041_021118f8[];

u32 data_ov041_021118a8[3] = {
    0x65746177,
    0x616d5f72,
    0x00000074,
};

u32 data_ov041_0211189c[3] = {
    0x5f74616d,
    0x6172696b,
    0x00000000,
};

u32 data_ov041_021118b4[3] = {
    (u32)data_ov041_0211188c,
    (u32)data_ov041_021118f8,
    0x10040200,
};
