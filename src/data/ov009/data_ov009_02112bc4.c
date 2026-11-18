/* ov009 cartridge data at 0x02112bc4, 24 bytes. */
#include "types.h"

extern u32 data_ov009_0211222c[];
extern u32 data_ov009_02112234[];
extern u32 data_ov009_02112238[];
extern u32 data_ov009_021122ec[];

#include "private/BTA_File.h"

BTA_File data_ov009_02112bc4 = {
    0x005b,
    0,
    (fx32 *)&data_ov009_02112234,
    (u16 *)&data_ov009_0211222c,
    (fx32 *)&data_ov009_021122ec,
    0x0001,
    0,
    (BTAMaterialRecord *)&data_ov009_02112238,
};
