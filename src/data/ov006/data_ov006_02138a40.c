/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138a48[];
extern struct OamAttr data_ov006_02138a68[];

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr * sSmartballSideSprites[2] = {
    (struct OamAttr *)&data_ov006_02138a48,
    (struct OamAttr *)&data_ov006_02138a68,
};
#pragma section obj end
