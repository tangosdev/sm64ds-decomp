/*
 * Minigame-overlay data at 0x0213ade8..0x0213adf0.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

#pragma define_section obj ".data.gap45b" ".data.gap45b" ".data.gap45b" far_data
#pragma section obj begin
u32 data_ov006_0213ade8[2] = {
    0x79655f6d,
    0x006c5f65,
};
#pragma section obj end
