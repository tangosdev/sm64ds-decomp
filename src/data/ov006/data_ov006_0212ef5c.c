/*
 * Minigame-overlay read-only table at 0x0212ef5c..0x0212ef9c.
 *
 * Values are the cartridge words; a word the module relocates
 * is written as its target symbol so the link re-creates it.
 */
#include "types.h"

#pragma define_section obj ".rodata.sound_row_views" ".rodata.sound_row_views" ".rodata.sound_row_views" far_data
#pragma section obj begin
int sSoundScrollRowLengthAlt[4] = {
    0x00000004,
    0x00000004,
    0x00000005,
    0x00000005,
};
#pragma section obj end
