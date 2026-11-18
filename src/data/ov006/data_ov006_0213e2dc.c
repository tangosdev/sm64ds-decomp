/*
 * Minigame-overlay data at 0x0213e2dc..0x0213e2e8.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

int sRouletteResultIndex = 0xffffffff;
