/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02137620;
extern struct OamAttr data_ov006_02137628;
extern struct OamAttr data_ov006_0213763c[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballPakkunSprites[3] = {
    (struct OamAttr *)&data_ov006_02137620,
    (struct OamAttr *)&data_ov006_02137628,
    (struct OamAttr *)&data_ov006_0213763c,
};
#pragma section obj end
