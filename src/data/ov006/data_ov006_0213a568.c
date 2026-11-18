/* Word table, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213a550;
extern struct OamAttr data_ov006_0213a558[];
extern struct OamAttr data_ov006_0213a578[];
extern struct OamAttr data_ov006_0213a598[];

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
int sAmidaLaneWantSprites[4] = {
    (int)&data_ov006_0213a598,
    (int)&data_ov006_0213a550,
    (int)&data_ov006_0213a578,
    (int)&data_ov006_0213a558,
};
#pragma section obj end
