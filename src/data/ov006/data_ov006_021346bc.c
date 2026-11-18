/*
 * Minigame-overlay data at 0x021346bc..0x02134b3c.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr data_ov006_021346c0[];

#pragma define_section obj ".data.gap13" ".data.gap13" ".data.gap13" far_data
#pragma section obj begin
struct OamAttr * data_ov006_021346bc[1] = { (struct OamAttr *)&data_ov006_021346c0 };
#pragma section obj end
