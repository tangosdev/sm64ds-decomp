/*
 * Minigame-overlay data at 0x0213b2f4..0x0213b31c.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

#pragma define_section obj ".data.gap50" ".data.gap50" ".data.gap50" far_data
#pragma section obj begin
int sTrmpLinePushStrongPitchMid = 0x00000118;
#pragma section obj end
