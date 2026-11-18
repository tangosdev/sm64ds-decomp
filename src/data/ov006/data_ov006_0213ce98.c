/* Word table, 5 entries. */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 sLuigiBoardModes[5] = {
    0x00030201,
    0x00000300,
    0x00000400,
    0x00000000,
    0x00000000,
};
#pragma section obj end
