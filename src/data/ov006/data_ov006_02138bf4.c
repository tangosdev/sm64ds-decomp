/* Pointers, 1 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138bf8[];

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr * sSmartballTimerSpriteAlt[1] = { (struct OamAttr *)&data_ov006_02138bf8 };
#pragma section obj end
