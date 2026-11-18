/* Pointers, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213505c[];
extern struct OamAttr data_ov006_0213507c[];

#pragma define_section obj ".data.gap18" ".data.gap18" ".data.gap18" far_data
#pragma section obj begin
struct OamAttr * sPanelHudSprites[2] = {
    (struct OamAttr *)&data_ov006_0213505c,
    (struct OamAttr *)&data_ov006_0213507c,
};
#pragma section obj end
