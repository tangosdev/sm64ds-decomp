/* Word table, 7 entries. */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 sPanelScrambleFlipsAlt[7] = {
    0x02010101,
    0x01010102,
    0x03030202,
    0x03030303,
    0x03030303,
    0x04040404,
    0x00000004,
};
#pragma section obj end
