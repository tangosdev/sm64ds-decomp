/* Pointers, 12 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138764;
extern struct OamAttr data_ov006_0213876c;
extern struct OamAttr data_ov006_02138774;
extern struct OamAttr data_ov006_0213877c;
extern struct OamAttr data_ov006_02138784;
extern struct OamAttr data_ov006_0213878c;
extern struct OamAttr data_ov006_02138794;
extern struct OamAttr data_ov006_0213879c;
extern struct OamAttr data_ov006_021387d4[];
extern struct OamAttr data_ov006_02138834[];
extern struct OamAttr data_ov006_021388ac[];
extern struct OamAttr data_ov006_0213893c[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballSparkSpritesAlt[12] = {
    (struct OamAttr *)&data_ov006_0213893c,
    (struct OamAttr *)&data_ov006_02138834,
    (struct OamAttr *)&data_ov006_021388ac,
    (struct OamAttr *)&data_ov006_021387d4,
    (struct OamAttr *)&data_ov006_0213879c,
    (struct OamAttr *)&data_ov006_02138794,
    (struct OamAttr *)&data_ov006_0213878c,
    (struct OamAttr *)&data_ov006_0213877c,
    (struct OamAttr *)&data_ov006_02138764,
    (struct OamAttr *)&data_ov006_02138774,
    (struct OamAttr *)&data_ov006_02138784,
    (struct OamAttr *)&data_ov006_0213876c,
};
#pragma section obj end
