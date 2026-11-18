/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213767c;
extern struct OamAttr data_ov006_0213768c[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballWingSprites[2] = {
    (struct OamAttr *)&data_ov006_0213768c,
    (struct OamAttr *)&data_ov006_0213767c,
};
#pragma section obj end
