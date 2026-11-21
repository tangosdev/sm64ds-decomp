#include "types.h"

extern u32 data_ov020_0211385c[];
extern u32 data_ov020_02113a18[];
extern u32 data_ov020_02113a38[];
extern u32 data_ov020_02113a58[];
extern u32 data_ov020_02113a78[];

u32 data_ov020_02113c20[2] = {
    0x00000205,
    (u32)data_ov020_0211385c,
};

u32 data_ov020_02113c28[8] = {
    0x00000220, (u32)data_ov020_02113a18, 0x00000240, (u32)data_ov020_02113a38,
    0x00000260, (u32)data_ov020_02113a58, 0x00000280, (u32)data_ov020_02113a78,
};
