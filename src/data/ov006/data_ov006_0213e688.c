/*
 * Minigame-overlay data at 0x0213e688..0x0213eaa8.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"
#include "OamAttr.h"

struct OamAttr data_ov006_0213e688[4] = {
    {0x00e8, 0x81e8, 0x0006, 0x0000},
    {0x80e8, 0x8008, 0x000a, 0x0000},
    {0x4008, 0x81e8, 0x0086, 0x0000},
    {0x0008, 0x4008, 0x008a, 0xffff},
};
