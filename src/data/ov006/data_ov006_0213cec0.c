/* Word table, 8 entries. */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 sLuigiWantedPicks[8] = {
    0x02010300,
    0x02010300,
    0x00020003,
    0x00020103,
    0x01020301,
    0x01020103,
    0x01020100,
    0x00000001,
};
#pragma section obj end
