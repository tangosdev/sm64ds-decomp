/*
 * Minigame-overlay data at 0x0213ab6c..0x0213aba8.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap42" ".data.gap42" ".data.gap42" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213ab6c = { 0x00e0, 0xc1d8, 0x1018, 0xffff };
#pragma section obj end
