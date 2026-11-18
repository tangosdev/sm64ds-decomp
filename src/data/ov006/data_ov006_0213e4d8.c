/*
 * Minigame-overlay data at 0x0213e4d8..0x0213e4e0.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
unsigned char sSlot1Payouts[8] = {
    0x0f, 0x0a, 0x0a, 0x05, 0x05, 0x14, 0x00, 0x00,
};
#pragma section obj end
