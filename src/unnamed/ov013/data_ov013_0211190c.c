#include "types.h"

extern u32 data_ov013_02111780[];
extern u32 data_ov013_02111a3c[];
extern u32 data_ov013_02111784[];

u32 data_ov013_0211190c[3] = {
    (u32)&data_ov013_02111780,
    0x00000c0a,
    (u32)&data_ov013_02111a3c,
};

u32 data_ov013_02111918[4] = {
    0x00000000,
    0x00000000,
    0x06ef0100,
    (u32)&data_ov013_02111784,
};
