/* ov014 cartridge data at 0x02113e50, 28 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113e50 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113e50 begin
u32 data_ov014_02113e50[7] = {
    0x0b86e225, 0x04b00e10, 0x04e2e225, 0x04b00f3c, 0x04e2e225, 0x05780ed8, 0x04e2e225,
};
#pragma section data_ov014_02113e50 end
