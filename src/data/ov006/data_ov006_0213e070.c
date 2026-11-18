/* Pointers, 9 entries. */
#include "types.h"

extern u32 data_ov006_0213ddc4[];
extern u32 data_ov006_0213ddd4[];
extern u32 data_ov006_0213dde4[];
extern u32 data_ov006_0213de0c[];
extern u32 data_ov006_0213de7c[];
extern u32 data_ov006_0213de98[];
extern u32 data_ov006_0213deb4[];
extern u32 data_ov006_021427ac[];
extern int data_ov006_02142804[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
void * sPanelPuzzles[9] = {
    (void *)&data_ov006_0213dde4,
    (void *)&data_ov006_0213ddc4,
    (void *)&data_ov006_0213ddd4,
    (void *)&data_ov006_021427ac,
    (void *)&data_ov006_0213de7c,
    (void *)&data_ov006_0213de98,
    (void *)&data_ov006_0213deb4,
    (void *)&data_ov006_02142804,
    (void *)&data_ov006_0213de0c,
};
#pragma section obj end
