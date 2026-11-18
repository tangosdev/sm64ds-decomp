/* Pointers, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02136d38;
extern struct OamAttr data_ov006_02136d50[];
extern struct OamAttr data_ov006_02136d60[];
extern struct OamAttr data_ov006_02136d78[];

#pragma define_section obj ".data.gap22" ".data.gap22" ".data.gap22" far_data
#pragma section obj begin
struct OamAttr * sPanelCursorSprites[4] = {
    (struct OamAttr *)&data_ov006_02136d38,
    (struct OamAttr *)&data_ov006_02136d50,
    (struct OamAttr *)&data_ov006_02136d60,
    (struct OamAttr *)&data_ov006_02136d78,
};
#pragma section obj end
