/*
 * Minigame-overlay data at 0x02136974..0x02136bd4.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr data_ov006_02136980[];
extern struct OamAttr data_ov006_02136990[];
extern struct OamAttr data_ov006_021369a0[];

#pragma define_section obj ".data.gap21" ".data.gap21" ".data.gap21" far_data
#pragma section obj begin
struct OamAttr * data_ov006_02136974[3] = {
    (struct OamAttr *)&data_ov006_02136990,
    (struct OamAttr *)&data_ov006_02136980,
    (struct OamAttr *)&data_ov006_021369a0,
};
#pragma section obj end
