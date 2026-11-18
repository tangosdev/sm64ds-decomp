/* Pointers, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138190[];
extern struct OamAttr data_ov006_021381a8[];
extern struct OamAttr data_ov006_021381c0[];
extern struct OamAttr data_ov006_021381d8[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballLineSpritesGerman[4] = {
    (struct OamAttr *)&data_ov006_021381c0,
    (struct OamAttr *)&data_ov006_021381a8,
    (struct OamAttr *)&data_ov006_02138190,
    (struct OamAttr *)&data_ov006_021381d8,
};
#pragma section obj end
