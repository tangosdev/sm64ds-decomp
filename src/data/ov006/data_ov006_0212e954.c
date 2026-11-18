/*
 * Minigame-overlay read-only table at 0x0212e954..0x0212ebac.
 *
 * Values are the cartridge words; a word the module relocates
 * is written as its target symbol so the link re-creates it.
 */
#include "types.h"

#pragma define_section obj ".rodata.gap3" ".rodata.gap3" ".rodata.gap3" far_data
#pragma section obj begin
int sMCarlo2StartFaceCounts[10] = {
    0x00000008,
    0x00000008,
    0x00000008,
    0x00000008,
    0x00000008,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
#pragma section obj end
