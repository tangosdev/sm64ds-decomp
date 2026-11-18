/* ov014 cartridge data at 0x02113f20, 44 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113f20 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02113f20 begin
u32 data_ov014_02113f20[11] = {
    0xfe3ee225, 0x03e810cc, 0xfe3ee225, 0x04b01004, 0xfa24e225, 0x04b01676,
    0xfa24e225, 0x05781612, 0xfa24e225, 0x05dc154a, 0xfa24e225,
};
#pragma section data_ov014_02113f20 end
