/*
 * Minigame-overlay read-only table at 0x0212efc4..0x0212f0f0.
 *
 * Values are the cartridge words; a word the module relocates
 * is written as its target symbol so the link re-creates it.
 */
#include "types.h"

#pragma define_section obj ".rodata.after_sound" ".rodata.after_sound" ".rodata.after_sound" far_data
#pragma section obj begin
unsigned char sTeresaBigBooHoverPicks[4] = {
    0x02, 0x03, 0x00, 0x00,
};
#pragma section obj end
