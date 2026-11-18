/*
 * Minigame-overlay data at 0x02134218..0x0213424c.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr data_ov006_02134200[];
extern struct OamAttr data_ov006_02134224[];
extern struct OamAttr data_ov006_02134274[];

#pragma define_section obj ".data.gap10" ".data.gap10" ".data.gap10" far_data
#pragma section obj begin
void * data_ov006_02134218[3] = {
    (void *)&data_ov006_02134274,
    (void *)&data_ov006_02134224,
    (void *)&data_ov006_02134200,
};
#pragma section obj end
