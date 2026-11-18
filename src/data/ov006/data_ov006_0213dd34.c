/*
 * Minigame-overlay data at 0x0213dd34..0x0213dd40.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

u32 sPanelFlipFrameSprites[3] = {
    0x01050403,
    0x02010000,
    0x00000304,
};
