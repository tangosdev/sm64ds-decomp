#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

extern const u32 data_ov053_0211194c[];
extern const u32 data_ov053_02111384[];

u32 data_ov053_021123e4[4] = {
    0x00000000,
    0x054603e8,
    0x80000000,
    0x05780000,
};

u32 data_ov053_021123d4[4] = {
    0x00000012,
    0x0000012c,
    0x00000000,
    0xffff0000,
};

u32 data_ov053_021123f4[4] = {
    0x0000005a,
    (u32)&data_ov053_0211194c,
    0x00000001,
    (u32)&data_ov053_02111384,
};
