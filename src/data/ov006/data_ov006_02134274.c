/*
 * Minigame-overlay data at 0x02134274..0x021342bc.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap10b" ".data.gap10b" ".data.gap10b" far_data
#pragma section obj begin
struct OamAttr data_ov006_02134274[9] = {
    { 0x00f5, 0x01f4, 0x11ee, 0x0000 },
    { 0x0000, 0x01fa, 0x11ef, 0x0000 },
    { 0x00f7, 0x01fd, 0x11ee, 0x0000 },
    { 0x0000, 0x0001, 0x11ee, 0x0000 },
    { 0x00ff, 0x01f1, 0x11ee, 0x0000 },
    { 0x0005, 0x01f9, 0x11ee, 0x0000 },
    { 0x00f9, 0x0002, 0x11ee, 0x0000 },
    { 0x00fa, 0x01f5, 0x11ef, 0x0000 },
    { 0x00f3, 0x01fa, 0x11ef, 0xffff },
};
#pragma section obj end
