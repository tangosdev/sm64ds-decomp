#include "types.h"

extern u32 data_ov010_02112864[];
extern u32 data_ov010_02112288[];
extern u32 data_ov010_02112580[];

u32 data_ov010_021123d8[3] = {
    0x00002602,
    (u32)data_ov010_02112864,
    0x00000303,
};

u32 data_ov010_021123e4[4] = {
    (u32)data_ov010_02112288,
    0x00000b04,
    (u32)data_ov010_02112580,
    0x0000050b,
};
