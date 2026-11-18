/*
 * Minigame-overlay data at 0x0213b0ec..0x0213b0f4.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

#pragma define_section obj ".data.gap49" ".data.gap49" ".data.gap49" far_data
#pragma section obj begin
int sTrmp3DLives = 0x00000003;
#pragma section obj end
