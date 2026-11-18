/* ov014 cartridge data at 0x02113d90, 24 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113d90 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113d90 begin
u32 data_ov014_02113d90[6] = {
    0x113be225, 0xf6210bba, 0x11fce225, 0xf5c30bbe, 0x12bee225, 0xf5600bc1,
};
#pragma section data_ov014_02113d90 end
