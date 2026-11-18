/* Pointers, 3 entries. */
#include "types.h"

extern u8 _ZTI15dScMgSnowball_c[];
extern struct OamAttr * sMgTextSpritesItalian[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
void * data_ov006_02140000[3] = {
    (void *)&sMgTextSpritesItalian,
    0,
    (void *)&_ZTI15dScMgSnowball_c,
};
#pragma section obj end
