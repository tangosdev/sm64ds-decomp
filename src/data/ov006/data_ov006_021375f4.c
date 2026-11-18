/* Word table, 2 entries. */
#include "types.h"

extern u32 data_ov006_021375e4;
extern struct OamAttr data_ov006_021375ec;

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
int sPachinkoFlyerSprites[2] = {
    (int)&data_ov006_021375ec,
    (int)&data_ov006_021375e4,
};
#pragma section obj end
