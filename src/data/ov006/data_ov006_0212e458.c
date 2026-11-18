/* Byte table, 8 bytes. */
#include "types.h"

#pragma define_section obj ".rodata.gap0" ".rodata.gap0" ".rodata.gap0" far_data
#pragma section obj begin
unsigned char sCurlingScoreDelays[8] = {
    0x78, 0x60, 0x48, 0x30, 0x18, 0x00, 0x00, 0x00,
};
#pragma section obj end
