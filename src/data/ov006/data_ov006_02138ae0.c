/* Pointers, 6 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138ad0;
extern struct OamAttr data_ov006_02138ad8;
extern struct OamAttr data_ov006_02138af8[];
extern struct OamAttr data_ov006_02138b28[];
extern struct OamAttr data_ov006_02138b60[];
extern struct OamAttr data_ov006_02138b98[];

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr * data_ov006_02138ae0[6] = {
    (struct OamAttr *)&data_ov006_02138ad8,
    (struct OamAttr *)&data_ov006_02138af8,
    (struct OamAttr *)&data_ov006_02138b60,
    (struct OamAttr *)&data_ov006_02138b98,
    (struct OamAttr *)&data_ov006_02138b28,
    (struct OamAttr *)&data_ov006_02138ad0,
};
#pragma section obj end
