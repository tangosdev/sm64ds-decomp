/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02137a48;
extern struct OamAttr data_ov006_02137a50;
extern struct OamAttr data_ov006_02137a58;

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * data_ov006_02137a60[3] = {
    (struct OamAttr *)&data_ov006_02137a58,
    (struct OamAttr *)&data_ov006_02137a50,
    (struct OamAttr *)&data_ov006_02137a48,
};
#pragma section obj end
