/*
 * Minigame-overlay data at 0x0213d580..0x0213d588.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern void * _Z23dScMgMCarlo_c_classInitv();

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 g_profile_MG_MCARLO[2] = {
    (u32)&_Z23dScMgMCarlo_c_classInitv,
    0x017d017d,
};
#pragma section obj end
