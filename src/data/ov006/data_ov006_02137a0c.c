/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02137a18[];
extern struct OamAttr data_ov006_02137a28[];
extern struct OamAttr data_ov006_02137a38[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballMarkerSprites[3] = {
    (struct OamAttr *)&data_ov006_02137a28,
    (struct OamAttr *)&data_ov006_02137a18,
    (struct OamAttr *)&data_ov006_02137a38,
};
#pragma section obj end
