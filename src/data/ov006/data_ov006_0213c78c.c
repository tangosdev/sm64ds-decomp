/*
 * Minigame-overlay data at 0x0213c78c..0x0213c794.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct dScMg3DEsp_c * _Z22dScMg3DEsp_c_classInitv();

#pragma define_section obj ".data.gap53" ".data.gap53" ".data.gap53" far_data
#pragma section obj begin
u32 g_profile_MG_3DESP[2] = {
    (u32)&_Z22dScMg3DEsp_c_classInitv,
    0x01850185,
};
#pragma section obj end
