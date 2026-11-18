/* Pointers, 12 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02137500;
extern struct OamAttr data_ov006_02137508;
extern struct OamAttr data_ov006_02137510;
extern struct OamAttr data_ov006_02137518;
extern struct OamAttr data_ov006_02137520;
extern struct OamAttr data_ov006_02137528;
extern struct OamAttr data_ov006_02137530;
extern struct OamAttr data_ov006_02137538;
extern struct OamAttr data_ov006_02137540;
extern struct OamAttr data_ov006_02137548;
extern struct OamAttr data_ov006_02137550;
extern struct OamAttr data_ov006_02137558;

#pragma define_section obj ".data.gap28" ".data.gap28" ".data.gap28" far_data
#pragma section obj begin
struct OamAttr * sHanaBackgroundParts[12] = {
    (struct OamAttr *)&data_ov006_02137558,
    (struct OamAttr *)&data_ov006_02137550,
    (struct OamAttr *)&data_ov006_02137548,
    (struct OamAttr *)&data_ov006_02137540,
    (struct OamAttr *)&data_ov006_02137538,
    (struct OamAttr *)&data_ov006_02137530,
    (struct OamAttr *)&data_ov006_02137528,
    (struct OamAttr *)&data_ov006_02137520,
    (struct OamAttr *)&data_ov006_02137518,
    (struct OamAttr *)&data_ov006_02137510,
    (struct OamAttr *)&data_ov006_02137508,
    (struct OamAttr *)&data_ov006_02137500,
};
#pragma section obj end
