/* Word table, 5 entries. */
#include "types.h"

#pragma define_section obj ".rodata.gap0" ".rodata.gap0" ".rodata.gap0" far_data
#pragma section obj begin
int sCurlingSlideShiftLeft[5] = {
    0x00000004,
    0x00000005,
    0x00000006,
    0x00000007,
    0x00000008,
};
#pragma section obj end
