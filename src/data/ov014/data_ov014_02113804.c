/* ov014 cartridge data at 0x02113804, 28 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113804 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02113804 begin
u32 data_ov014_02113804[7] = {
    0x076c02ee, 0x00000000, 0xffff0000, 0x05a90033, 0x019702ee, 0xe8e40000,
    0xffff0000,
};
#pragma section data_ov014_02113804 end
