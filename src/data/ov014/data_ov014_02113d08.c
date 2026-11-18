/* ov014 cartridge data at 0x02113d08, 40 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113d08 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113d08 begin
u32 data_ov014_02113d08[10] = {
    0xe89d1e29, 0xe72d0371, 0xe9e71e29, 0xee3e03e7, 0xe60b1e29,
    0xed6f03f7, 0xe8741e29, 0xfe5b03e8, 0xec781e29, 0x07d002ed,
};
#pragma section data_ov014_02113d08 end
