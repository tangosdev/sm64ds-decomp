/* ov056 cartridge data at 0x02112158, 32 bytes. */
#include "types.h"

#pragma define_section data_ov056_02112158 ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov056_02112158 begin
int data_ov056_02112158[8] = {
    0x000034dc, 0x000034fa, 0x00003519, 0x00003537, 0x00003555, 0x00003574, 0x00003592, 0x000035b0,
};
#pragma section data_ov056_02112158 end
