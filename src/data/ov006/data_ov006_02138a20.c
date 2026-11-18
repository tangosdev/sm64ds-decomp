/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138a10;
extern struct OamAttr data_ov006_02138a18;

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr * sSmartballSpringSprites[2] = {
    (struct OamAttr *)&data_ov006_02138a18,
    (struct OamAttr *)&data_ov006_02138a10,
};
#pragma section obj end
