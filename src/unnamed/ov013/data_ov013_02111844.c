#include "types.h"

extern u32 data_ov013_02111b94[];
extern u32 data_ov013_021117a4[];
extern u32 data_ov013_02111918[];

u32 data_ov013_02111844[7] = {
    (u32)&data_ov013_02111b94,
    (u32)&data_ov013_021117a4,
    0x06f306fa,
    0x06fc06fb,
    (u32)&data_ov013_02111918,
    0x06400007,
    0x0000000f,
};

u32 data_ov013_02111860[12] = {
    0xff100000, 0x0010ffff, 0x7fa5ff02, 0xff020012, 0x001480fe, 0xffffff0f,
    0xff070023, 0x002affff, 0xffffff0b, 0xff0d0035, 0x0042ffff, 0xffffff0a,
};

u32 data_ov013_02111890[16] = {
    0xedb5002a, 0xe3d105c3, 0x00000000, 0x0cff0000, 0xedb5002a, 0xfbd70523, 0x80000000, 0x0c330000,
    0xe462002a, 0xf7590564, 0x40000000, 0x4c770000, 0xe57000ed, 0xf89404fb, 0x40000000, 0x24000000,
};
