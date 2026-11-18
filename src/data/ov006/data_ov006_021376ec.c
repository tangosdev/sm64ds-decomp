/* Pointers, 10 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213769c;
extern struct OamAttr data_ov006_021376a4;
extern struct OamAttr data_ov006_021376ac[];
extern struct OamAttr data_ov006_021376bc[];
extern struct OamAttr data_ov006_021376cc[];
extern struct OamAttr data_ov006_021376dc[];
extern struct OamAttr data_ov006_02137714[];
extern struct OamAttr data_ov006_02137764[];
extern struct OamAttr data_ov006_021377b4[];
extern struct OamAttr data_ov006_021378a4[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sRouletteStripSprites[10] = {
    (struct OamAttr *)&data_ov006_02137714,
    (struct OamAttr *)&data_ov006_02137764,
    (struct OamAttr *)&data_ov006_021376cc,
    (struct OamAttr *)&data_ov006_021376bc,
    (struct OamAttr *)&data_ov006_021376dc,
    (struct OamAttr *)&data_ov006_021376ac,
    (struct OamAttr *)&data_ov006_0213769c,
    (struct OamAttr *)&data_ov006_021376a4,
    (struct OamAttr *)&data_ov006_021377b4,
    (struct OamAttr *)&data_ov006_021378a4,
};
#pragma section obj end
