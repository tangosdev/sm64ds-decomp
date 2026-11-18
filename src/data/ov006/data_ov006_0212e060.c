/* 16-bit table, 4 entries. */
#include "types.h"

#pragma define_section obj ".rodata.before_coin_tables" ".rodata.before_coin_tables" ".rodata.before_coin_tables" far_data
#pragma section obj begin
unsigned short sTrmpLineDiffuse[4] = {
    0x7fff, 0x13ff, 0x011f, 0x0000,
};
#pragma section obj end
