#include "types.h"

extern u32 data_ov010_021121cc[];
extern u32 data_ov010_021121c4[];
extern u32 data_ov010_0211221c[];
extern u32 data_ov010_021121fc[];

u32 data_ov010_021124c0[1] = {
    (u32)data_ov010_021121cc,
};

u32 data_ov010_021124c4[4] = {
    0x00000000,
    0x06a40300,
    (u32)data_ov010_021121c4,
    0x00000000,
};

u32 data_ov010_021124d4[6] = {
    0x05780200, (u32)data_ov010_0211221c, 0x00000000,
    0x05dc0102, (u32)data_ov010_021121fc, 0x00000000,
};
