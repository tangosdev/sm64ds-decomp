/*
 * Minigame-overlay data at 0x0213c6d4..0x0213c6dc.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

#pragma define_section obj ".data.gap51" ".data.gap51" ".data.gap51" far_data
#pragma section obj begin
u32 data_ov006_0213c6d4[2] = {
    0x00600060,
    0x006000a0,
};
#pragma section obj end
