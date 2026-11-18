/* ov014 cartridge data at 0x021133ec, 24 bytes. */
#include "types.h"

#pragma define_section data_ov014_021133ec ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_021133ec begin
u32 data_ov014_021133ec[6] = {
    0xff080000, 0x00087fff, 0x7fffff05, 0xff1a000d, 0x00277fff, 0xffffff0a,
};
#pragma section data_ov014_021133ec end
