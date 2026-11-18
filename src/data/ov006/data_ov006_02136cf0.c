/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02136ce0;
extern struct OamAttr data_ov006_02136ce8;
extern struct OamAttr data_ov006_02136cfc[];

#pragma define_section obj ".data.gap22" ".data.gap22" ".data.gap22" far_data
#pragma section obj begin
struct OamAttr * data_ov006_02136cf0[3] = {
    (struct OamAttr *)&data_ov006_02136ce8,
    (struct OamAttr *)&data_ov006_02136cfc,
    (struct OamAttr *)&data_ov006_02136ce0,
};
#pragma section obj end
