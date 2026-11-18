/* ov034 cartridge data at 0x02114114, 56 bytes. */
#include "types.h"

extern u32 data_ov034_02114080[];
extern u32 data_ov034_02114084[];
extern u32 data_ov034_02114088[];
extern u32 data_ov034_0211409c[];
extern u32 data_ov034_021140b0[];
extern u32 data_ov034_021140c0[];
extern u32 data_ov034_0211414c[];

u32 data_ov034_02114114[14] = {
    0x00000104, (u32)&data_ov034_021140b0, 0x0000020b, (u32)&data_ov034_02114080,
    0x00000501, (u32)&data_ov034_0211414c, 0x00000108, (u32)&data_ov034_0211409c,
    0x0000020c, (u32)&data_ov034_02114088, 0x0000010e, (u32)&data_ov034_02114084,
    0x0000010a, (u32)&data_ov034_021140c0,
};
