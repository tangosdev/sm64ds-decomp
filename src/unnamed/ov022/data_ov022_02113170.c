#include "types.h"

extern u32 data_ov022_02113160[];
extern u32 data_ov022_021131f8[];

u32 data_ov022_02113170[3] = {
    0xff020000,
    0x00027fff,
    0x7fffff02,
};

u32 data_ov022_0211317c[3] = {
    (u32)data_ov022_02113160,
    (u32)data_ov022_021131f8,
    0x04e20000,
};
