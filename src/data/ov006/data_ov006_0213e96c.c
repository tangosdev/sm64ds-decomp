/* Pointers, 14 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213e4e8;
extern struct OamAttr data_ov006_0213e4f0;
extern struct OamAttr data_ov006_0213e510;
extern u8 data_ov006_0213e518[];
extern struct OamAttr data_ov006_0213e558;
extern struct OamAttr data_ov006_0213e568;
extern u8 data_ov006_0213e570[];
extern struct OamAttr data_ov006_0213e688[];
extern struct OamAttr data_ov006_0213e848[];
extern struct OamAttr data_ov006_0213e888[];
extern struct OamAttr data_ov006_0213e8a8[];
extern struct OamAttr data_ov006_0213e908[];
extern struct OamAttr data_ov006_0213e928[];
extern u32 data_ov006_0213ea60[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
void * sSlot1Sprites[14] = {
    (void *)&data_ov006_0213e848,
    (void *)&data_ov006_0213e688,
    (void *)&data_ov006_0213e928,
    (void *)&data_ov006_0213e888,
    (void *)&data_ov006_0213e8a8,
    (void *)&data_ov006_0213ea60,
    (void *)&data_ov006_0213e4e8,
    (void *)&data_ov006_0213e4f0,
    (void *)&data_ov006_0213e558,
    (void *)&data_ov006_0213e568,
    (void *)&data_ov006_0213e518,
    (void *)&data_ov006_0213e570,
    (void *)&data_ov006_0213e908,
    (void *)&data_ov006_0213e510,
};
#pragma section obj end
