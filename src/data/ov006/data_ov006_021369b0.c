/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_021369bc[];
extern struct OamAttr data_ov006_021369cc[];
extern struct OamAttr data_ov006_021369dc[];

#pragma define_section obj ".data.gap21" ".data.gap21" ".data.gap21" far_data
#pragma section obj begin
struct OamAttr * data_ov006_021369b0[3] = {
    (struct OamAttr *)&data_ov006_021369cc,
    (struct OamAttr *)&data_ov006_021369bc,
    (struct OamAttr *)&data_ov006_021369dc,
};
#pragma section obj end
