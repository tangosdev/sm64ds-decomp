/*
 * Minigame-overlay data at 0x02134ecc..0x02134ef0.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr data_ov006_02134ed0[];

#pragma define_section obj ".data.gap15b" ".data.gap15b" ".data.gap15b" far_data
#pragma section obj begin
struct OamAttr * sTrmpLifeIconSprite[1] = { (struct OamAttr *)&data_ov006_02134ed0 };
#pragma section obj end
