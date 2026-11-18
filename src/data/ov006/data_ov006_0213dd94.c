/*
 * Minigame-overlay data at 0x0213dd94..0x0213e24c.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 sPanelScrambleWidths[4] = {
    0x06060606,
    0x06060606,
    0x06060606,
    0x00060606,
};
#pragma section obj end
