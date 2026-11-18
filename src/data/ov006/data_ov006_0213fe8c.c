/*
 * Minigame-overlay data at 0x0213fe8c..0x0213fec8.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u8 _ZTI10dScMgBSC_c[];
extern struct OamAttr data_ov006_0213fda8[];
extern struct OamAttr data_ov006_0213fdb8[];
extern struct OamAttr data_ov006_0213fdc8[];
extern struct OamAttr data_ov006_0213fdd8[];
extern struct OamAttr data_ov006_0213fde8[];
extern struct OamAttr data_ov006_0213fdf8[];
extern struct OamAttr data_ov006_0213fe08[];
extern struct OamAttr data_ov006_0213fe18[];
extern struct OamAttr data_ov006_0213fe28[];
extern struct OamAttr data_ov006_0213fe38[];
extern struct OamAttr data_ov006_0213fe48[];
extern struct OamAttr data_ov006_0213fe58[];
extern struct OamAttr data_ov006_0213fe68[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
void * sBscCardSprites[15] = {
    (void *)&data_ov006_0213fda8,
    (void *)&data_ov006_0213fe68,
    (void *)&data_ov006_0213fdb8,
    (void *)&data_ov006_0213fdc8,
    (void *)&data_ov006_0213fdd8,
    (void *)&data_ov006_0213fe48,
    (void *)&data_ov006_0213fde8,
    (void *)&data_ov006_0213fdf8,
    (void *)&data_ov006_0213fe08,
    (void *)&data_ov006_0213fe18,
    (void *)&data_ov006_0213fe28,
    (void *)&data_ov006_0213fe38,
    (void *)&data_ov006_0213fe58,
    0,
    (void *)&_ZTI10dScMgBSC_c,
};
#pragma section obj end
