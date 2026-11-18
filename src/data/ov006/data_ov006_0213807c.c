/* Pointers, 9 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138034;
extern struct OamAttr data_ov006_0213803c;
extern struct OamAttr data_ov006_02138044;
extern struct OamAttr data_ov006_0213804c;
extern struct OamAttr data_ov006_02138054;
extern struct OamAttr data_ov006_0213805c;
extern struct OamAttr data_ov006_02138064;
extern struct OamAttr data_ov006_0213806c;
extern struct OamAttr data_ov006_02138074;

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballCellRowSprites[9] = {
    (struct OamAttr *)&data_ov006_02138074,
    (struct OamAttr *)&data_ov006_0213806c,
    (struct OamAttr *)&data_ov006_02138064,
    (struct OamAttr *)&data_ov006_0213805c,
    (struct OamAttr *)&data_ov006_02138054,
    (struct OamAttr *)&data_ov006_0213804c,
    (struct OamAttr *)&data_ov006_02138044,
    (struct OamAttr *)&data_ov006_0213803c,
    (struct OamAttr *)&data_ov006_02138034,
};
#pragma section obj end
