/* ov014 cartridge data at 0x02113980, 24 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113980 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02113980 begin
u32 data_ov014_02113980[6] = {
    0x05a90033, 0x019702ee, 0xe8e40000, 0xffff0000, 0xea460032, 0x155c007d,
};
#pragma section data_ov014_02113980 end
