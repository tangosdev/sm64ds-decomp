/* Pointers, 1 entries. */
#include "types.h"

extern u32 data_ov006_021379ac[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballWingBodySprites[1] = { (struct OamAttr *)&data_ov006_021379ac };
#pragma section obj end
