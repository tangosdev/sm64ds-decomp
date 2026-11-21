#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

extern u32 data_ov056_02112bc0[];

u32 data_ov056_02112bec[3] = {
    0x5f74616d,
    0x6167616e,
    0x00006572,
};

u32 data_ov056_02112be0[3] = {
    0x5f74616d,
    0x65746177,
    0x00000072,
};

u32 data_ov056_02112bf8[3] = {
    (u32)&data_ov056_02112bc0[4],
    (u32)&data_ov056_02112bc0[29],
    0x27100000,
};
