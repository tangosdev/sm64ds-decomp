/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_021350a8[];
extern struct OamAttr data_ov006_021350b8[];
extern struct OamAttr data_ov006_021350c8[];

#pragma define_section obj ".data.gap18" ".data.gap18" ".data.gap18" far_data
#pragma section obj begin
struct OamAttr * data_ov006_0213509c[3] = {
    (struct OamAttr *)&data_ov006_021350b8,
    (struct OamAttr *)&data_ov006_021350a8,
    (struct OamAttr *)&data_ov006_021350c8,
};
#pragma section obj end
