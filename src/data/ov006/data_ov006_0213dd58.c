/* Pointers, 3 entries. */
#include "types.h"

extern u32 data_ov006_0213df30[];
extern u32 data_ov006_0213e140[];
extern u32 data_ov006_0213e1fc[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
void * sPanelPuzzleYPerMode[3] = {
    (void *)&data_ov006_0213df30,
    (void *)&data_ov006_0213e140,
    (void *)&data_ov006_0213e1fc,
};
#pragma section obj end
