#include "types.h"

extern const u32 data_ov053_021111cc[];
extern u32 data_ov053_021111b4[];
extern const u32 data_ov053_02111dc4[];
extern const u32 data_ov053_021111e0[];

u32 data_ov053_0211241c[3] = {
    0x000000b5,
    (u32)&data_ov053_021111cc,
    (u32)&data_ov053_021111b4,
};

u32 data_ov053_02112428[3] = {
    (u32)&data_ov053_02111dc4,
    0x00000001,
    (u32)&data_ov053_021111e0,
};
