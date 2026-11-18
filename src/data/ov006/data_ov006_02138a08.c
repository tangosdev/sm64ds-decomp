/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_021389f8;
extern struct OamAttr data_ov006_02138a00;

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr * sSmartballPropellerSprites[2] = {
    (struct OamAttr *)&data_ov006_02138a00,
    (struct OamAttr *)&data_ov006_021389f8,
};
#pragma section obj end
