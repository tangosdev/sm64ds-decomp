#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

extern const u32 data_ov026_02112498[];
extern const u32 data_ov026_02112494[];
extern const u32 data_ov026_021125cc[];
extern const u32 data_ov026_021124ac[];
extern u32 data_ov026_021137c0[];
extern u32 data_ov026_02113450[];
extern u32 data_ov026_02112e80[];
extern u32 data_ov026_02112de0[];
extern u32 data_ov026_02112ef8[];
extern u32 data_ov026_02112dd8[];
extern u32 data_ov026_02112f28[];

u32 data_ov026_02112ef8[6] = {
    0x0000005b, (u32)data_ov026_02112498, (u32)data_ov026_02112494, (u32)data_ov026_021125cc,
    0x00000002, (u32)data_ov026_021124ac,
};

u32 data_ov026_02112ee0[6] = {
    0x00002d00, (u32)data_ov026_021137c0, 0x00003605, (u32)data_ov026_02113450,
    0x00000160, (u32)data_ov026_02112e80,
};

u32 data_ov026_02112f10[6] = {
    (u32)data_ov026_02112de0, (u32)data_ov026_02112ef8, 0x0cae0000,
    (u32)data_ov026_02112dd8, (u32)data_ov026_02112f28, 0x0ea90000,
};
