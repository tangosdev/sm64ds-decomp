/* Word table, 2 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02136dfc;
extern struct OamAttr data_ov006_02136e04;

#pragma define_section obj ".data.gap22" ".data.gap22" ".data.gap22" far_data
#pragma section obj begin
int sPachinkoTargetSprites[2] = {
    (int)&data_ov006_02136e04,
    (int)&data_ov006_02136dfc,
};
#pragma section obj end
