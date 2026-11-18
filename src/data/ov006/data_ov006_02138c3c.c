/* Pointers, 1 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138c40;

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr * sRouletteRacerSprites[1] = { (struct OamAttr *)&data_ov006_02138c40 };
#pragma section obj end
