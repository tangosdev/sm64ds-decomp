/*
 * Minigame-overlay data at 0x02133f18..0x02133f24.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr data_ov006_02133f1c;

#pragma define_section obj ".data.gap5" ".data.gap5" ".data.gap5" far_data
#pragma section obj begin
struct OamAttr * sMCarloHudSprite[1] = { (struct OamAttr *)&data_ov006_02133f1c };
#pragma section obj end
