/* Pointers, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov006_021380b0[];
extern struct OamAttr data_ov006_021380c8[];
extern struct OamAttr data_ov006_021380e0[];
extern struct OamAttr data_ov006_021380f8[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballLineSprites[4] = {
    (struct OamAttr *)&data_ov006_021380e0,
    (struct OamAttr *)&data_ov006_021380c8,
    (struct OamAttr *)&data_ov006_021380b0,
    (struct OamAttr *)&data_ov006_021380f8,
};
#pragma section obj end
