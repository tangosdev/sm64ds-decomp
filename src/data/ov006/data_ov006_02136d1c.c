/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02136d0c;
extern struct OamAttr data_ov006_02136d14;
extern struct OamAttr data_ov006_02136d28[];

#pragma define_section obj ".data.gap22" ".data.gap22" ".data.gap22" far_data
#pragma section obj begin
struct OamAttr * data_ov006_02136d1c[3] = {
    (struct OamAttr *)&data_ov006_02136d14,
    (struct OamAttr *)&data_ov006_02136d28,
    (struct OamAttr *)&data_ov006_02136d0c,
};
#pragma section obj end
