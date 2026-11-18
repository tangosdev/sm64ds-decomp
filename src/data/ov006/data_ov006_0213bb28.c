/* Pointers, 4 entries. */
#include "types.h"

extern u32 data_ov006_0213b9b0;
extern u32 data_ov006_0213b9b8;
extern u32 data_ov006_0213b9d0;
extern u32 data_ov006_0213b9e4[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
unsigned char * sBomroomBombLitSprites[4] = {
    (unsigned char *)&data_ov006_0213b9b0,
    (unsigned char *)&data_ov006_0213b9d0,
    (unsigned char *)&data_ov006_0213b9b8,
    (unsigned char *)&data_ov006_0213b9e4,
};
#pragma section obj end
