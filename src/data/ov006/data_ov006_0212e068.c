/* 16-bit table, 4 entries. */
#include "types.h"

#pragma define_section obj ".rodata.before_coin_tables" ".rodata.before_coin_tables" ".rodata.before_coin_tables" far_data
#pragma section obj begin
unsigned short sTrmpLineAmbient[4] = {
    0x3def, 0x018c, 0x318c, 0x0000,
};
#pragma section obj end
