/* ov014 cartridge data at 0x021138d8, 24 bytes. */
#include "types.h"

#pragma define_section data_ov014_021138d8 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021138d8 begin
u32 data_ov014_021138d8[6] = {
    0x00000000, 0xffff0000, 0xef330017, 0xfd7a03e8, 0x20000000, 0xffff0000,
};
#pragma section data_ov014_021138d8 end
