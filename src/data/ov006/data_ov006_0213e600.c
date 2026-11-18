/* Word table, 5 entries. */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
int sSlotReelScrollSpeeds[5] = {
    0x00006000,
    0x00007000,
    0x00008000,
    0x00009000,
    0x0000a000,
};
#pragma section obj end
