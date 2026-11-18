/*
 * Minigame-overlay data at 0x0213bc50..0x0213bc58.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern void * _Z21dScMgCard_c_classInitv();

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 g_profile_MG_CARD[2] = {
    (u32)&_Z21dScMgCard_c_classInitv,
    0x017b017b,
};
#pragma section obj end
