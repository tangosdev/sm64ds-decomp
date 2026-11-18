/*
 * Minigame-overlay data at 0x0213d338..0x0213d350.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u32 data_ov006_0213d384[];
extern u32 data_ov006_0213d3c4[];
extern u32 data_ov006_0213d40c[];

unsigned short *sMemory2CardPositionsPerDifficulty[3] = {
    (unsigned short *)&data_ov006_0213d384,
    (unsigned short *)&data_ov006_0213d3c4,
    (unsigned short *)&data_ov006_0213d40c,
};
