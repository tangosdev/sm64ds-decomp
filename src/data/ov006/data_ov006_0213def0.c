/* Pointers, 8 entries. */
#include "types.h"

extern unsigned int data_ov006_0213dc94[];
extern struct OamAttr data_ov006_0213dda4[];
extern struct OamAttr data_ov006_0213ddb4[];
extern struct OamAttr data_ov006_0213ddf4[];
extern struct OamAttr data_ov006_0213e094[];
extern struct OamAttr data_ov006_0213e0bc[];
extern struct OamAttr data_ov006_0213e0e4[];
extern struct OamAttr data_ov006_0213e174[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
void * sPanelParticleSprites[8] = {
    (void *)&data_ov006_0213e174,
    (void *)&data_ov006_0213e094,
    (void *)&data_ov006_0213e0bc,
    (void *)&data_ov006_0213dda4,
    (void *)&data_ov006_0213e0e4,
    (void *)&data_ov006_0213ddf4,
    (void *)&data_ov006_0213ddb4,
    (void *)&data_ov006_0213dc94,
};
#pragma section obj end
