/*
 * Minigame-overlay data at 0x0213f9e4..0x0213fa0c.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u8 _ZTI13dScMgTeresa_c[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
int sTeresaPaintMask[10] = {
    0x00111100,
    0x01111110,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x01111110,
    0x00111100,
    0x00000000,
    (int)&_ZTI13dScMgTeresa_c,
};
#pragma section obj end
