/* Pointers, 5 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02136d98[];
extern struct OamAttr data_ov006_02136da8[];
extern struct OamAttr data_ov006_02136db8[];
extern struct OamAttr data_ov006_02136dc8[];
extern struct OamAttr data_ov006_02136dd8[];

#pragma define_section obj ".data.gap22" ".data.gap22" ".data.gap22" far_data
#pragma section obj begin
struct OamAttr * data_ov006_02136de8[5] = {
    (struct OamAttr *)&data_ov006_02136dd8,
    (struct OamAttr *)&data_ov006_02136dc8,
    (struct OamAttr *)&data_ov006_02136db8,
    (struct OamAttr *)&data_ov006_02136da8,
    (struct OamAttr *)&data_ov006_02136d98,
};
#pragma section obj end
