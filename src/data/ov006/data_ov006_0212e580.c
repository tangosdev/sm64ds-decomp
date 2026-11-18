/*
 * One object for data_ov006_0212e580, data_ov006_0212e584 (8 bytes): the
 * linker starts every input section on a 4-byte boundary, so these tables
 * cannot be separate.
 */
#include "types.h"

#pragma define_section obj ".rodata.gap1" ".rodata.gap1" ".rodata.gap1" far_data
#pragma section obj begin
u32 data_ov006_0212e580[2] = {
    0x00000000, 0x00001000,
};
#pragma section obj end
