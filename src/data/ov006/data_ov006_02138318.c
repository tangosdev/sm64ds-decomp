/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138300;
extern struct OamAttr data_ov006_02138308;
extern struct OamAttr data_ov006_02138310;

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballAnaSprites[3] = {
    (struct OamAttr *)&data_ov006_02138310,
    (struct OamAttr *)&data_ov006_02138308,
    (struct OamAttr *)&data_ov006_02138300,
};
#pragma section obj end
