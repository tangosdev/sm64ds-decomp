/* 0x020d7e38: Ov007Display::palettes -- the six NCCL palettes.
 * void *[6], cartridge pointers. */
#include "types.h"

#pragma define_section sec_020d7e38 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section sec_020d7e38 begin
extern u8 data_ov007_020d8a8c[];
extern u8 data_ov007_020d8cb8[];
extern u8 data_ov007_020d8ee4[];
extern u8 data_ov007_020d9110[];
extern u8 data_ov007_020d933c[];
extern u8 data_ov007_020d9568[];
void *data_ov007_020d7e38[6] = {
    (void *)&data_ov007_020d9568,
    (void *)&data_ov007_020d8a8c,
    (void *)&data_ov007_020d8cb8,
    (void *)&data_ov007_020d8ee4,
    (void *)&data_ov007_020d9110,
    (void *)&data_ov007_020d933c,
};
#pragma section sec_020d7e38 end
