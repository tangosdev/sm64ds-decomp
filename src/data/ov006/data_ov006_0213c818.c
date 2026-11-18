/* Word table, 5 entries. */
#include "types.h"

extern u32 data_ov006_0213c6d4[];
extern u32 data_ov006_0213c7bc[];
extern u32 data_ov006_0213c7e4[];
extern u32 data_ov006_0213c804[];
extern u32 data_ov006_0213c82c[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
int volatile data_ov006_0213c818[5] = {
    (int volatile)&data_ov006_0213c6d4,
    (int volatile)&data_ov006_0213c7bc,
    (int volatile)&data_ov006_0213c7e4,
    (int volatile)&data_ov006_0213c804,
    (int volatile)&data_ov006_0213c82c,
};
#pragma section obj end
