/*
 * Minigame-overlay data at 0x0213ec04..0x0213ec10.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr data_ov006_021375fc;
extern struct OamAttr data_ov006_02137604;
extern struct OamAttr data_ov006_0213760c;

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr * sSmartballDokanSprites[3] = {
    (struct OamAttr *)&data_ov006_021375fc,
    (struct OamAttr *)&data_ov006_02137604,
    (struct OamAttr *)&data_ov006_0213760c,
};
#pragma section obj end
