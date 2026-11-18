/*
 * Minigame-overlay data at 0x021374d0..0x021374f4.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap27" ".data.gap27" ".data.gap27" far_data
#pragma section obj begin
struct OamAttr data_ov006_021374d0 = { 0x80e0, 0xc1f0, 0x5014, 0xffff };
#pragma section obj end
