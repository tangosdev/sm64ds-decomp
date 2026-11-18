/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213706c;
extern struct OamAttr data_ov006_02137074;

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr * data_ov006_0213744c[2] = {
    (struct OamAttr *)&data_ov006_02137074,
    (struct OamAttr *)&data_ov006_0213706c,
};
#pragma section obj end
