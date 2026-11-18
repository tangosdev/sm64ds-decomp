/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02135024[];
extern struct OamAttr data_ov006_02135034[];
extern struct OamAttr data_ov006_02135044[];

#pragma define_section obj ".data.gap18" ".data.gap18" ".data.gap18" far_data
#pragma section obj begin
struct OamAttr * data_ov006_02135018[3] = {
    (struct OamAttr *)&data_ov006_02135034,
    (struct OamAttr *)&data_ov006_02135024,
    (struct OamAttr *)&data_ov006_02135044,
};
#pragma section obj end
