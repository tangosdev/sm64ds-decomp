/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02137664;
extern struct OamAttr data_ov006_0213766c;

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballKinokoTurnSprites[2] = {
    (struct OamAttr *)&data_ov006_0213766c,
    (struct OamAttr *)&data_ov006_02137664,
};
#pragma section obj end
