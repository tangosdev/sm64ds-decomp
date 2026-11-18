/* ov014 cartridge data at 0x0211341c, 24 bytes. */
#include "types.h"

#pragma define_section data_ov014_0211341c ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_0211341c begin
u32 data_ov014_0211341c[6] = {
    0x1ab1ff0e, 0xe6f107bf, 0xed13ff0e, 0xfd1203e8, 0x0bb6483f, 0xee0b10c4,
};
#pragma section data_ov014_0211341c end
