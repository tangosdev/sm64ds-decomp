/* ov014 cartridge data at 0x02113e6c, 28 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113e6c ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113e6c begin
u32 data_ov014_02113e6c[7] = {
    0x05dc0e10, 0x04e2e225, 0x05780d48, 0x04e2e225, 0x04b00ce4, 0x04e2e225, 0x03e80d48,
};
#pragma section data_ov014_02113e6c end
