/* Word table, 7 entries. */
#include "types.h"

extern u8 _ZTI11dScMgJump_c[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
int sJumpNcgByLanguage[7] = {
    0x000000d0,
    0x000000d2,
    0x000000cd,
    0x000000ce,
    0x000000cf,
    0x00000000,
    (int)&_ZTI11dScMgJump_c,
};
#pragma section obj end
