/* Pointers, 5 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213a5b8;
extern struct OamAttr data_ov006_0213a5c0;
extern struct OamAttr data_ov006_0213a5c8;
extern struct OamAttr data_ov006_0213a5d0;
extern struct OamAttr data_ov006_0213a5d8;

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
struct OamAttr * sCurlingBitSprites[5] = {
    (struct OamAttr *)&data_ov006_0213a5d8,
    (struct OamAttr *)&data_ov006_0213a5d0,
    (struct OamAttr *)&data_ov006_0213a5c8,
    (struct OamAttr *)&data_ov006_0213a5c0,
    (struct OamAttr *)&data_ov006_0213a5b8,
};
#pragma section obj end
