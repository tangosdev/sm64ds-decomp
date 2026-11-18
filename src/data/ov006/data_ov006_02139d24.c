/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02139d2c;
extern struct OamAttr data_ov006_02139d34[];

#pragma define_section obj ".data.gap33" ".data.gap33" ".data.gap33" far_data
#pragma section obj begin
struct OamAttr * sSnowballCourseMeterSprites[2] = {
    (struct OamAttr *)&data_ov006_02139d2c,
    (struct OamAttr *)&data_ov006_02139d34,
};
#pragma section obj end
