/* Word table, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02133890;
extern struct OamAttr data_ov006_02133898;
extern struct OamAttr data_ov006_021339bc[];

#pragma define_section obj ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section obj begin
int data_ov006_021338c4[3] = {
    (int)&data_ov006_02133890,
    (int)&data_ov006_02133898,
    (int)&data_ov006_021339bc,
};
#pragma section obj end
