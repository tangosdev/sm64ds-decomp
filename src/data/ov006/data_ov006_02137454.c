/* Pointers, 5 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02136f7c;
extern struct OamAttr data_ov006_02136f8c;
extern struct OamAttr data_ov006_02136f94;
extern struct OamAttr data_ov006_02136fa4;
extern struct OamAttr data_ov006_02137164[];

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr * sMgTextSpritesItalian[5] = {
    (struct OamAttr *)&data_ov006_02136fa4,
    (struct OamAttr *)&data_ov006_02136f94,
    (struct OamAttr *)&data_ov006_02136f8c,
    (struct OamAttr *)&data_ov006_02136f7c,
    (struct OamAttr *)&data_ov006_02137164,
};
#pragma section obj end
