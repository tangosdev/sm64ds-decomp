/* Pointers, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213a380[];
extern struct OamAttr data_ov006_0213a3a0[];
extern struct OamAttr data_ov006_0213a3b8[];
extern struct OamAttr data_ov006_0213a3d0[];

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
struct OamAttr * sAmidaBoardAnimSprites[4] = {
    (struct OamAttr *)&data_ov006_0213a380,
    (struct OamAttr *)&data_ov006_0213a3b8,
    (struct OamAttr *)&data_ov006_0213a3a0,
    (struct OamAttr *)&data_ov006_0213a3d0,
};
#pragma section obj end
