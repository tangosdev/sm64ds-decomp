/*
 * Minigame-overlay data at 0x02137500..0x02137590.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap28" ".data.gap28" ".data.gap28" far_data
#pragma section obj begin
struct OamAttr data_ov006_02137500 = { 0x00f0, 0x81f0, 0x5090, 0xffff };
#pragma section obj end
