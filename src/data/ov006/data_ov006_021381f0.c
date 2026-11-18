/* Pointers, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138200[];
extern struct OamAttr data_ov006_02138218[];
extern struct OamAttr data_ov006_02138230[];
extern struct OamAttr data_ov006_02138248[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballLineSpritesItalian[4] = {
    (struct OamAttr *)&data_ov006_02138230,
    (struct OamAttr *)&data_ov006_02138218,
    (struct OamAttr *)&data_ov006_02138200,
    (struct OamAttr *)&data_ov006_02138248,
};
#pragma section obj end
