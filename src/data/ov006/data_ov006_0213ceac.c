/* Word table, 5 entries. */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 sLuigiBoardMovers[5] = {
    0x00000000,
    0x00000300,
    0x06020000,
    0x02000700,
    0x00060000,
};
#pragma section obj end
