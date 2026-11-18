/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138a90[];
extern struct OamAttr data_ov006_02138ab0[];

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr * sSmartballSideSpritesAlt[2] = {
    (struct OamAttr *)&data_ov006_02138a90,
    (struct OamAttr *)&data_ov006_02138ab0,
};
#pragma section obj end
