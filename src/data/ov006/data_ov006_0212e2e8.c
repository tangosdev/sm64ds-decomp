/* 16-bit table, 16 entries. */
#include "types.h"

#pragma define_section obj ".rodata.before_coin_tables" ".rodata.before_coin_tables" ".rodata.before_coin_tables" far_data
#pragma section obj begin
unsigned short sBomroomSpawnDelays[16] = {
    0x0060, 0x0060, 0x0060, 0x0048, 0x0030, 0x0060, 0x0048, 0x0048,
    0x0060, 0x0020, 0x0060, 0x0048, 0x0060, 0x0060, 0x0060, 0x0060,
};
#pragma section obj end
