/*
 * Minigame-overlay data at 0x0213d954..0x0213d960.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

unsigned short sPachinkoPopScores[6] = {
    0x0064, 0x00c8, 0x0190, 0x0320, 0x0640, 0x0c80,
};
