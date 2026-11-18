/* Word table, 7 entries. */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 sPanelScrambleFlipEnabledAlt[7] = {
    0x00010001,
    0x02000201,
    0x00010200,
    0x02000100,
    0x00020000,
    0x00020001,
    0x00000000,
};
#pragma section obj end
