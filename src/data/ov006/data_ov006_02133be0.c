/*
 * Minigame-overlay data at 0x02133be0..0x02133f00.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

#pragma define_section obj ".data.gap4" ".data.gap4" ".data.gap4" far_data
#pragma section obj begin
unsigned char data_ov006_02133be0[16] = {
    0x00, 0x40, 0xe8, 0xc1, 0x17, 0x30, 0x00, 0x00,
    0xe0, 0x00, 0x00, 0x80, 0x12, 0x40, 0xff, 0xff,
};
#pragma section obj end
