/*
 * Minigame-overlay read-only table at 0x0212e450..0x0212e574.
 *
 * Values are the cartridge words; a word the module relocates
 * is written as its target symbol so the link re-creates it.
 */
#include "types.h"

#pragma define_section obj ".rodata.gap0" ".rodata.gap0" ".rodata.gap0" far_data
#pragma section obj begin
unsigned char sCurlingBitAStates[4] = {
    0x01, 0x02, 0x00, 0x00,
};
#pragma section obj end
