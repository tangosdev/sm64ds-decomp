/*
 * Minigame-overlay data at 0x02133600..0x0213386c.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section obj begin
struct OamAttr data_ov006_02133600[2] = {
    { 0x80e8, 0x41fc, 0x011f, 0x0000 },
    { 0x8008, 0x01fc, 0x019f, 0xffff },
};
#pragma section obj end
