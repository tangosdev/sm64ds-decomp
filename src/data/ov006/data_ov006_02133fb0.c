/*
 * Minigame-overlay data at 0x02133fb0..0x02134028.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap6" ".data.gap6" ".data.gap6" far_data
#pragma section obj begin
struct OamAttr data_ov006_02133fb0[2] = {
    { 0x40f8, 0x81e0, 0x0140, 0x0000 },
    { 0x40f8, 0x8000, 0x0144, 0xffff },
};
#pragma section obj end
