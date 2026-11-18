/* Word table, 5 entries. */
#include "types.h"

#pragma define_section obj ".rodata.gap0" ".rodata.gap0" ".rodata.gap0" far_data
#pragma section obj begin
int sCurlingSlideMinLeft[5] = {
    0x00000180,
    0x00000140,
    0x00000100,
    0x000000c0,
    0x00000080,
};
#pragma section obj end
