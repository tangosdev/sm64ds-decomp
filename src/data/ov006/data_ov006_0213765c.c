/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213764c;
extern struct OamAttr data_ov006_02137654;

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballKinokoSprites[2] = {
    (struct OamAttr *)&data_ov006_02137654,
    (struct OamAttr *)&data_ov006_0213764c,
};
#pragma section obj end
