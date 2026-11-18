/* Word table, 5 entries. */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 sLuigiBoardFlags[5] = {
    0x01000000,
    0x00000001,
    0x00000000,
    0x00000000,
    0x00000000,
};
#pragma section obj end
