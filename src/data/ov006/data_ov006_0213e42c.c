/*
 * Minigame-overlay data at 0x0213e42c..0x0213e448.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u8 _ZTI19dScMgSingle3DBase_c[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
int sSingle3DCommonNcgByLanguage[7] = {
    0x000000c2,
    0x000000c4,
    0x000000bf,
    0x000000c0,
    0x000000c1,
    0x00000000,
    (int)&_ZTI19dScMgSingle3DBase_c,
};
#pragma section obj end
