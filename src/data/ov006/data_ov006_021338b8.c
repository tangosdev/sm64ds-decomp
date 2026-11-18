/* Word table, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_021338ec[];
extern struct OamAttr data_ov006_0213390c[];
extern struct OamAttr data_ov006_0213399c[];

#pragma define_section obj ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section obj begin
int data_ov006_021338b8[3] = {
    (int)&data_ov006_021338ec,
    (int)&data_ov006_0213390c,
    (int)&data_ov006_0213399c,
};
#pragma section obj end
