/* Pointers, 16 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138324;
extern struct OamAttr data_ov006_0213832c;
extern struct OamAttr data_ov006_02138334;
extern struct OamAttr data_ov006_0213833c;
extern struct OamAttr data_ov006_02138344;
extern struct OamAttr data_ov006_0213834c;
extern struct OamAttr data_ov006_02138354;
extern struct OamAttr data_ov006_0213835c;
extern struct OamAttr data_ov006_021383a4[];
extern struct OamAttr data_ov006_0213841c[];
extern struct OamAttr data_ov006_02138494[];
extern struct OamAttr data_ov006_0213850c[];
extern struct OamAttr data_ov006_02138584[];
extern struct OamAttr data_ov006_021385fc[];
extern struct OamAttr data_ov006_02138674[];
extern struct OamAttr data_ov006_021386ec[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballSparkSprites[16] = {
    (struct OamAttr *)&data_ov006_02138674,
    (struct OamAttr *)&data_ov006_021386ec,
    (struct OamAttr *)&data_ov006_02138584,
    (struct OamAttr *)&data_ov006_021383a4,
    (struct OamAttr *)&data_ov006_0213841c,
    (struct OamAttr *)&data_ov006_02138494,
    (struct OamAttr *)&data_ov006_0213850c,
    (struct OamAttr *)&data_ov006_021385fc,
    (struct OamAttr *)&data_ov006_0213835c,
    (struct OamAttr *)&data_ov006_0213833c,
    (struct OamAttr *)&data_ov006_02138334,
    (struct OamAttr *)&data_ov006_0213834c,
    (struct OamAttr *)&data_ov006_02138324,
    (struct OamAttr *)&data_ov006_0213832c,
    (struct OamAttr *)&data_ov006_02138344,
    (struct OamAttr *)&data_ov006_02138354,
};
#pragma section obj end
