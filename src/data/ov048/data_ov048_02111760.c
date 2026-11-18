/* ov048 data_ov048_02111760 at 0x02111760 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov048_02111760[2] = {
    0x53504c43, 0x00020008,
};

#pragma section dc8_section end
