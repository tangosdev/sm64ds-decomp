/* Pointers, 5 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02139c44;
extern struct OamAttr data_ov006_02139c4c;
extern struct OamAttr data_ov006_02139c54;
extern struct OamAttr data_ov006_02139c5c;
extern struct OamAttr data_ov006_02139c64;

#pragma define_section obj ".data.gap33" ".data.gap33" ".data.gap33" far_data
#pragma section obj begin
struct OamAttr * sSnowballBitSprites[5] = {
    (struct OamAttr *)&data_ov006_02139c64,
    (struct OamAttr *)&data_ov006_02139c5c,
    (struct OamAttr *)&data_ov006_02139c54,
    (struct OamAttr *)&data_ov006_02139c4c,
    (struct OamAttr *)&data_ov006_02139c44,
};
#pragma section obj end
