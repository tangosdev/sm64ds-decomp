/* Pointers, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02137aa8[];
extern struct OamAttr data_ov006_02137ab8[];
extern struct OamAttr data_ov006_02137ac8[];
extern struct OamAttr data_ov006_02137ad8[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSoundWinSpriteBSprites[4] = {
    (struct OamAttr *)&data_ov006_02137ad8,
    (struct OamAttr *)&data_ov006_02137ac8,
    (struct OamAttr *)&data_ov006_02137ab8,
    (struct OamAttr *)&data_ov006_02137aa8,
};
#pragma section obj end
