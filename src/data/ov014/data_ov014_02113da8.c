/* ov014 cartridge data at 0x02113da8, 28 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113da8 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113da8 begin
u32 data_ov014_02113da8[7] = {
    0x1375e225, 0xf5020bc3, 0x1427e225, 0xf49e0bc6, 0xfd68e225, 0x15820000, 0xfea2e225,
};
#pragma section data_ov014_02113da8 end
