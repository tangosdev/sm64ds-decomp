/*
 * Minigame-overlay data at 0x021389f8..0x02138c48.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr data_ov006_021389f8 = { 0x00e0, 0xc1e0, 0x1110, 0xffff };
#pragma section obj end
