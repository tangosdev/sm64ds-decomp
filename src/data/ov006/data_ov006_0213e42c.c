/*
 * Minigame-overlay data at 0x0213e42c..0x0213e440.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

int sSingle3DCommonNcgByLanguage[5] = {
    0x000000c2, 0x000000c4, 0x000000bf, 0x000000c0, 0x000000c1,
};
/* _ZTV19dScMgSingle3DBase_c starts 8 bytes past this table; those bytes are
   its {offset-to-top, type_info} header. */
