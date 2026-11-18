/*
 * Minigame-overlay data at 0x0213cb8c..0x0213cb98.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

int sJumpRenderScale[3] = {
    0x00002800,
    0x00002800,
    0x00001000,
};
