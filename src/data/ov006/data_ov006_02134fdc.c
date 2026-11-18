/*
 * Minigame-overlay data at 0x02134fdc..0x021350d8.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u32 data_ov006_02134fe8;
extern struct OamAttr data_ov006_02134ff8[];
extern struct OamAttr data_ov006_02135008[];

#pragma define_section obj ".data.gap18" ".data.gap18" ".data.gap18" far_data
#pragma section obj begin
void * data_ov006_02134fdc[3] = {
    (void *)&data_ov006_02134ff8,
    (void *)&data_ov006_02134fe8,
    (void *)&data_ov006_02135008,
};
#pragma section obj end
