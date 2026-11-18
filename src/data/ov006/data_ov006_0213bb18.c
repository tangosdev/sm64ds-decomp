/* Pointers, 4 entries. */
#include "types.h"

extern u32 data_ov006_0213b9ac;
extern u32 data_ov006_0213b9c4;
extern u32 data_ov006_0213b9c8;
extern u32 data_ov006_0213b9dc[];

#pragma define_section obj ".data.w13b814" ".data.w13b814" ".data.w13b814" far_data
#pragma section obj begin
unsigned char * sBomroomBombSprites[4] = {
    (unsigned char *)&data_ov006_0213b9ac,
    (unsigned char *)&data_ov006_0213b9c8,
    (unsigned char *)&data_ov006_0213b9c4,
    (unsigned char *)&data_ov006_0213b9dc,
};
#pragma section obj end
