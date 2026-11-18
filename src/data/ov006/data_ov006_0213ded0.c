/* Pointers, 8 entries. */
#include "types.h"

extern u32 data_ov006_0213df50[];
extern u32 data_ov006_0213df74[];
extern u32 data_ov006_0213df98[];
extern u32 data_ov006_0213dfbc[];
extern u32 data_ov006_0213dfe0[];
extern u32 data_ov006_0213e004[];
extern u32 data_ov006_0213e028[];
extern u32 data_ov006_0213e04c[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
void * sPanelPuzzlesAlt[8] = {
    (void *)&data_ov006_0213df50,
    (void *)&data_ov006_0213df74,
    (void *)&data_ov006_0213df98,
    (void *)&data_ov006_0213dfbc,
    (void *)&data_ov006_0213dfe0,
    (void *)&data_ov006_0213e004,
    (void *)&data_ov006_0213e028,
    (void *)&data_ov006_0213e04c,
};
#pragma section obj end
