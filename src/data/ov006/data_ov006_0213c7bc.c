/*
 * Minigame-overlay data at 0x0213c7bc..0x0213c7c8.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

u32 data_ov006_0213c7bc[3] = {
    0x00600040,
    0x00600080,
    0x006000c0,
};
