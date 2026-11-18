/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138a28;
extern struct OamAttr data_ov006_02138a30;

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr * sSmartballSpringSpritesAlt[2] = {
    (struct OamAttr *)&data_ov006_02138a30,
    (struct OamAttr *)&data_ov006_02138a28,
};
#pragma section obj end
