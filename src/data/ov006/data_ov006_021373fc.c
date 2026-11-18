/* Pointers, 6 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02136e4c;
extern struct OamAttr data_ov006_02136e64;
extern struct OamAttr data_ov006_02136e6c;
extern struct OamAttr data_ov006_021370fc;
extern struct OamAttr data_ov006_02137144[];
extern struct OamAttr data_ov006_02137154[];

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr * sMgTextSpritesSpanish[6] = {
    (struct OamAttr *)&data_ov006_02136e64,
    (struct OamAttr *)&data_ov006_02136e6c,
    (struct OamAttr *)&data_ov006_02136e4c,
    (struct OamAttr *)&data_ov006_021370fc,
    (struct OamAttr *)&data_ov006_02137144,
    (struct OamAttr *)&data_ov006_02137154,
};
#pragma section obj end
