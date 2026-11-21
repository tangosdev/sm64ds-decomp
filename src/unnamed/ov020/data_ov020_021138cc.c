#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

extern const u32 data_ov020_02113508[];
extern const u32 data_ov020_021134ec[];
extern const u32 data_ov020_021134e8[];
extern const u32 data_ov020_021134e4[];

u32 data_ov020_021138e4[3] = {
    0xfb508226,
    0x08fc0000,
    0xf830063d,
};

u32 data_ov020_021138d8[3] = {
    (u32)data_ov020_02113508,
    0x00000001,
    (u32)data_ov020_021134ec,
};

u32 data_ov020_021138cc[3] = {
    0x0000005b,
    (u32)data_ov020_021134e8,
    (u32)data_ov020_021134e4,
};

u32 data_ov020_021138f0[3] = {
    0x076c04b0,
    0xf830e5ff,
    0x05780258,
};
