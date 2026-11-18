/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_021382d0;
extern struct OamAttr data_ov006_021382d8;

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballBallSprites[2] = {
    (struct OamAttr *)&data_ov006_021382d8,
    (struct OamAttr *)&data_ov006_021382d0,
};
#pragma section obj end
