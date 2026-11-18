/*
 * Minigame-overlay data at 0x0213c610..0x0213c62c.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u8 _ZTI14dScMgD3DBase_c[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
unsigned int sD3DOptionNcgByLanguage[7] = {
    0x000000de,
    0x000000df,
    0x000000db,
    0x000000dc,
    0x000000dd,
    0x00000000,
    (unsigned int)&_ZTI14dScMgD3DBase_c,
};
#pragma section obj end
