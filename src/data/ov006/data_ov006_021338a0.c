/* Word table, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_021338dc[];
extern struct OamAttr data_ov006_0213394c[];
extern struct OamAttr data_ov006_0213397c[];

#pragma define_section obj ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section obj begin
int data_ov006_021338a0[3] = {
    (int)&data_ov006_0213394c,
    (int)&data_ov006_021338dc,
    (int)&data_ov006_0213397c,
};
#pragma section obj end
