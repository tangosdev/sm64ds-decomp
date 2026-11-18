/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02137a84;
extern struct OamAttr data_ov006_02137a8c;
extern struct OamAttr data_ov006_02137a94;

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballPushSwitchSprites[3] = {
    (struct OamAttr *)&data_ov006_02137a94,
    (struct OamAttr *)&data_ov006_02137a8c,
    (struct OamAttr *)&data_ov006_02137a84,
};
#pragma section obj end
