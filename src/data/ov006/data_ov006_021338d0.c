/* Word table, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_021338fc[];
extern struct OamAttr data_ov006_0213391c[];
extern struct OamAttr data_ov006_021339dc[];

#pragma define_section obj ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section obj begin
int data_ov006_021338d0[3] = {
    (int)&data_ov006_0213391c,
    (int)&data_ov006_021338fc,
    (int)&data_ov006_021339dc,
};
#pragma section obj end
