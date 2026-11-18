/* Word table, 18 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02139eec[];
extern struct OamAttr data_ov006_02139f1c[];
extern struct OamAttr data_ov006_02139fac[];

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
u32 data_ov006_0213a2cc[18] = {
    (u32)&data_ov006_02139eec,
    0x00000005,
    (u32)&data_ov006_02139f1c,
    0x00000005,
    (u32)&data_ov006_02139eec,
    0x00000005,
    (u32)&data_ov006_02139fac,
    0x00000014,
    (u32)&data_ov006_02139eec,
    0x00000005,
    (u32)&data_ov006_02139f1c,
    0x00000005,
    (u32)&data_ov006_02139eec,
    0x00000005,
    (u32)&data_ov006_02139fac,
    0x00000005,
    (u32)&data_ov006_02139eec,
    0x00000005,
};
#pragma section obj end
