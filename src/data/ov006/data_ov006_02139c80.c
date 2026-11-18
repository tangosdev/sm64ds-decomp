/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02139c88[];
extern struct OamAttr data_ov006_02139cc8[];

#pragma define_section obj ".data.gap33" ".data.gap33" ".data.gap33" far_data
#pragma section obj begin
struct OamAttr * sSnowballObstacleSideSprites[2] = {
    (struct OamAttr *)&data_ov006_02139c88,
    (struct OamAttr *)&data_ov006_02139cc8,
};
#pragma section obj end
