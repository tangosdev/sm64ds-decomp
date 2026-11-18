/* 0x020d7e28: Ov007Display::characters -- the four NCCG character files.
 * void *[4], cartridge pointers. */
#include "types.h"

#pragma define_section sec_020d7e28 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section sec_020d7e28 begin
extern u8 data_ov007_020e2204[];
extern u8 data_ov007_020e6478[];
extern u8 data_ov007_020ea6ec[];
extern u8 data_ov007_020f295c[];
void *data_ov007_020d7e28[4] = {
    (void *)&data_ov007_020ea6ec,
    (void *)&data_ov007_020e2204,
    (void *)&data_ov007_020e6478,
    (void *)&data_ov007_020f295c,
};
#pragma section sec_020d7e28 end
