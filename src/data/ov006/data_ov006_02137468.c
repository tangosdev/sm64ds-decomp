/* Pointers, 7 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02136eb4;
extern struct OamAttr data_ov006_02136f24;
extern struct OamAttr data_ov006_02136f4c;
extern struct OamAttr data_ov006_02136fb4;
extern struct OamAttr data_ov006_0213700c;
extern struct OamAttr data_ov006_02137184[];
extern struct OamAttr data_ov006_02137294[];

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr * data_ov006_02137468[7] = {
    (struct OamAttr *)&data_ov006_02137184,
    (struct OamAttr *)&data_ov006_02136f4c,
    (struct OamAttr *)&data_ov006_02136eb4,
    (struct OamAttr *)&data_ov006_02136f24,
    (struct OamAttr *)&data_ov006_0213700c,
    (struct OamAttr *)&data_ov006_02137294,
    (struct OamAttr *)&data_ov006_02136fb4,
};
#pragma section obj end
