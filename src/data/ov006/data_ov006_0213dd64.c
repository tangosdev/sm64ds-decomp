/* Word table, 4 entries. */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 sPanelScrambleFlipEnabled[4] = {
    0x00010001,
    0x00000100,
    0x00010001,
    0x00000100,
};
#pragma section obj end
