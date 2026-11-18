/* ov029 cartridge data at 0x0211300c, 32 bytes. */
#include "types.h"

#pragma define_section data_ov029_0211300c ".data.w12ea0" ".data.w12ea0" ".data.w12ea0" far_data
#pragma section data_ov029_0211300c begin
u32 data_ov029_0211300c[8] = {
    0x53504c43, 0x00020008, 0x01000fc0, 0x000000ff, 0x01000fc5, 0x000000ff,
    0xffffffff, 0xffffffff,
};
#pragma section data_ov029_0211300c end
