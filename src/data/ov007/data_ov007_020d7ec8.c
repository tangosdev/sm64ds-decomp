/* 0x020d7ec8: bg layer screens: four NCSC files, then four unused entries.
 * int[8], cartridge pointers. */
#include "types.h"

#pragma define_section sec_020d7ec8 ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section sec_020d7ec8 begin
extern u8 data_ov007_020dbdbc[];
extern u8 data_ov007_020dd6c8[];
extern u8 data_ov007_020defdc[];
extern u8 data_ov007_020e08f0[];
int data_ov007_020d7ec8[8] = {
    0,
    (int)&data_ov007_020e08f0,
    (int)&data_ov007_020dd6c8,
    (int)&data_ov007_020defdc,
    (int)&data_ov007_020dbdbc,
    0,
    0,
    0,
};
#pragma section sec_020d7ec8 end
