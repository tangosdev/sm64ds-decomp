/* Word table, 4 entries. */
#include "types.h"

extern u32 data_ov001_020ab600;
extern u32 data_ov001_020ab6b0[];

extern const u32 data_ov001_020ab630[4] = {
    0x00000001,
    (u32)&data_ov001_020ab600,
    0x00000134,
    (u32)&data_ov001_020ab6b0,
};
