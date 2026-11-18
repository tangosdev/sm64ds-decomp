/* Pointers, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213a490;
extern struct OamAttr data_ov006_0213a498;
extern struct OamAttr data_ov006_0213a4a0;
extern struct OamAttr data_ov006_0213a4a8;

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
struct OamAttr * sAmidaPieceSprites[4] = {
    (struct OamAttr *)&data_ov006_0213a4a8,
    (struct OamAttr *)&data_ov006_0213a4a0,
    (struct OamAttr *)&data_ov006_0213a498,
    (struct OamAttr *)&data_ov006_0213a490,
};
#pragma section obj end
