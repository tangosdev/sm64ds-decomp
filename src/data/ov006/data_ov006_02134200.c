/*
 * Minigame-overlay data at 0x02134200..0x02134210.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap9b" ".data.gap9b" ".data.gap9b" far_data
#pragma section obj begin
struct OamAttr data_ov006_02134200[2] = {
    { 0x00f8, 0x01f8, 0x11ef, 0x0000 },
    { 0x0000, 0x01fd, 0x11ef, 0xffff },
};
#pragma section obj end
