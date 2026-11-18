/* Pointers, 1 entries. */
#include "types.h"

extern u8 data_ov006_0213ac40[];

#pragma define_section obj ".data.gap43" ".data.gap43" ".data.gap43" far_data
#pragma section obj begin
struct OamAttr * sSlotLamp2Sprite[1] = { (struct OamAttr *)&data_ov006_0213ac40 };
#pragma section obj end
