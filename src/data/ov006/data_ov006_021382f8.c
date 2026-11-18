/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_021382e8;
extern struct OamAttr data_ov006_021382f0;

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballBallSpritesAlt[2] = {
    (struct OamAttr *)&data_ov006_021382f0,
    (struct OamAttr *)&data_ov006_021382e8,
};
#pragma section obj end
