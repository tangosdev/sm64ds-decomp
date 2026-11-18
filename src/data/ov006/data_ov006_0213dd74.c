/* Word table, 4 entries. */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 sPanelScrambleFlips[4] = {
    0x02010101,
    0x03020202,
    0x04040303,
    0x00040404,
};
#pragma section obj end
