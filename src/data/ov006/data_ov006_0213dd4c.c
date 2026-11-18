/*
 * Minigame-overlay data at 0x0213dd4c..0x0213dd84.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u32 data_ov006_0213df10[];
extern u32 data_ov006_0213e10c[];
extern u32 data_ov006_0213e1b4[];

void *sPanelPuzzleXPerMode[3] = {
    (void *)&data_ov006_0213df10,
    (void *)&data_ov006_0213e10c,
    (void *)&data_ov006_0213e1b4,
};
