/* ov014 cartridge data at 0x021133dc, 16 bytes. */
#include "types.h"

#pragma define_section data_ov014_021133dc ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021133dc begin
u32 data_ov014_021133dc[4] = {
    0x1ab1ff0e,
    0xe6f107bf,
    0xed13ff0e,
    0xfd1203e8,
};
#pragma section data_ov014_021133dc end
