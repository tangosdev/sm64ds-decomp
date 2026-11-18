/*
 * Minigame-overlay data at 0x0213eed0..0x0213eefc.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u8 _ZTI16dScMgSmartball_c[];

u32 sSmartballCellStartOrder[11] = {
    0x00000003,
    0x00000008,
    0x00000006,
    0x00000001,
    0x00000004,
    0x00000009,
    0x00000002,
    0x00000007,
    0x00000005,
    0x00000000,
    (u32)&_ZTI16dScMgSmartball_c,
};
