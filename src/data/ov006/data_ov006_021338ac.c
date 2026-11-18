/* Word table, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213392c[];
extern struct OamAttr data_ov006_0213393c[];
extern struct OamAttr data_ov006_0213395c[];

#pragma define_section obj ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section obj begin
int data_ov006_021338ac[3] = {
    (int)&data_ov006_0213392c,
    (int)&data_ov006_0213393c,
    (int)&data_ov006_0213395c,
};
#pragma section obj end
