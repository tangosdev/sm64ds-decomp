/*
 * Minigame-overlay data at 0x02134cf8..0x02134d1c.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr data_ov006_02134cfc[];

#pragma define_section obj ".data.gap14" ".data.gap14" ".data.gap14" far_data
#pragma section obj begin
struct OamAttr * sJumpLifeIconSprite[1] = { (struct OamAttr *)&data_ov006_02134cfc };
#pragma section obj end
