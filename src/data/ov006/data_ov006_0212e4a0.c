/* Word table, 5 entries. */
#include "types.h"

#pragma define_section obj ".rodata.gap0" ".rodata.gap0" ".rodata.gap0" far_data
#pragma section obj begin
int sCurlingSlideShiftRight[5] = {
    0x00000008,
    0x00000007,
    0x00000006,
    0x00000006,
    0x00000004,
};
#pragma section obj end
