/* ov013 cartridge data at 0x02111824, 16 bytes. */
#include "types.h"

extern u32 data_ov013_02111814[];
extern u32 data_ov013_021119cc[];

u32 data_ov013_02111824[4] = {
    0x00000700,
    (u32)&data_ov013_021119cc,
    0x00000205,
    (u32)&data_ov013_02111814,
};
