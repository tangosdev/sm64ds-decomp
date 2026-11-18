/*
 * Minigame-overlay data at 0x0213c2e4..0x0213c304.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u8 _ZTI14dScMgCurling_c[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
int sCurlingAimMarkerSprites[8] = {
    0x80004000,
    0x00001008,
    0x80204000,
    0x0000100c,
    0x00000000,
    0xffff0000,
    0x00000000,
    (int)&_ZTI14dScMgCurling_c,
};
#pragma section obj end
