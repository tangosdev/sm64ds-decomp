/* Pointers, 10 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02136e74;
extern struct OamAttr data_ov006_02136ebc;
extern struct OamAttr data_ov006_02136ed4;
extern struct OamAttr data_ov006_02136f14;
extern struct OamAttr data_ov006_02136f54;
extern struct OamAttr data_ov006_02136fac;
extern struct OamAttr data_ov006_02136fdc;
extern struct OamAttr data_ov006_0213708c;
extern struct OamAttr data_ov006_02137134[];
extern struct OamAttr data_ov006_0213720c[];

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr * data_ov006_02137484[10] = {
    (struct OamAttr *)&data_ov006_02136fac,
    (struct OamAttr *)&data_ov006_02136ed4,
    (struct OamAttr *)&data_ov006_0213708c,
    (struct OamAttr *)&data_ov006_02136ebc,
    (struct OamAttr *)&data_ov006_02136f14,
    (struct OamAttr *)&data_ov006_02137134,
    (struct OamAttr *)&data_ov006_0213720c,
    (struct OamAttr *)&data_ov006_02136e74,
    (struct OamAttr *)&data_ov006_02136fdc,
    (struct OamAttr *)&data_ov006_02136f54,
};
#pragma section obj end
