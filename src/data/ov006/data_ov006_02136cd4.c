/*
 * Minigame-overlay data at 0x02136cd4..0x02136e14.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr data_ov006_02136cd8;

#pragma define_section obj ".data.gap22" ".data.gap22" ".data.gap22" far_data
#pragma section obj begin
int sPachinkoBallSprites[1] = { (int)&data_ov006_02136cd8 };
#pragma section obj end
