/* Pointers, 7 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213707c;
extern struct OamAttr data_ov006_02137094;
extern struct OamAttr data_ov006_0213709c;
extern struct OamAttr data_ov006_021370a4;
extern struct OamAttr data_ov006_021370ac;
extern struct OamAttr data_ov006_02137174[];
extern struct OamAttr data_ov006_0213723c[];

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr * data_ov006_02137430[7] = {
    (struct OamAttr *)&data_ov006_021370ac,
    (struct OamAttr *)&data_ov006_021370a4,
    (struct OamAttr *)&data_ov006_0213709c,
    (struct OamAttr *)&data_ov006_02137094,
    (struct OamAttr *)&data_ov006_02137174,
    (struct OamAttr *)&data_ov006_0213723c,
    (struct OamAttr *)&data_ov006_0213707c,
};
#pragma section obj end
