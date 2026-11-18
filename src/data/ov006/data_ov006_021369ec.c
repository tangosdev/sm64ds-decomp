/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_021369f8[];
extern struct OamAttr data_ov006_02136a08[];
extern struct OamAttr data_ov006_02136a18[];

#pragma define_section obj ".data.gap21" ".data.gap21" ".data.gap21" far_data
#pragma section obj begin
struct OamAttr * data_ov006_021369ec[3] = {
    (struct OamAttr *)&data_ov006_02136a08,
    (struct OamAttr *)&data_ov006_021369f8,
    (struct OamAttr *)&data_ov006_02136a18,
};
#pragma section obj end
