/*
 * Minigame-overlay data at 0x0213c75c..0x0213c764.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

#pragma define_section obj ".data.gap52" ".data.gap52" ".data.gap52" far_data
#pragma section obj begin
u32 data_ov006_0213c75c[2] = {
    0x6d5f696b,
    0x00757a69,
};
#pragma section obj end
