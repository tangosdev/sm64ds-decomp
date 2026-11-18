/*
 * Minigame-overlay read-only table at 0x0212ecec..0x0212ed14.
 *
 * Values are the cartridge words; a word the module relocates
 * is written as its target symbol so the link re-creates it.
 */
#include "types.h"

#pragma define_section obj ".rodata.gap9" ".rodata.gap9" ".rodata.gap9" far_data
#pragma section obj begin
u32 sPanelShakeOffsets[5] = {
    0x00000002,
    0xfffffffe,
    0x00000002,
    0xfffffffe,
    0x00000002,
};
#pragma section obj end
