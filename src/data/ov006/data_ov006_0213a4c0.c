/* Word table, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213a4d0[];
extern struct OamAttr data_ov006_0213a4e8[];
extern struct OamAttr data_ov006_0213a500[];
extern struct OamAttr data_ov006_0213a520[];

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
int sAmidaLaneResultSprites[4] = {
    (int)&data_ov006_0213a500,
    (int)&data_ov006_0213a4e8,
    (int)&data_ov006_0213a520,
    (int)&data_ov006_0213a4d0,
};
#pragma section obj end
