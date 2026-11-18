/* ov056 cartridge data at 0x02111a60, 24 bytes. */
#include "types.h"

#pragma define_section data_ov056_02111a60 ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov056_02111a60 begin
int data_ov056_02111a60[6] = {
    0x00000000, 0x0000001e, 0x0000003d, 0x0000005b, 0x00000079, 0x00000098,
};
#pragma section data_ov056_02111a60 end
