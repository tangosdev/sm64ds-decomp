/*
 * Minigame-overlay data at 0x0213d5b4..0x0213d5c8.
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
int sMCarloCardNcgByLanguage[5] = {
    0x000000ba,
    0x000000bc,
    0x000000b7,
    0x000000b8,
    0x000000b9,
};
#pragma section obj end
