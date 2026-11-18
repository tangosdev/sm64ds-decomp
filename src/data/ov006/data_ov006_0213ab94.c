/* Pointers, 5 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213ab6c;
extern struct OamAttr data_ov006_0213ab74;
extern struct OamAttr data_ov006_0213ab7c;
extern struct OamAttr data_ov006_0213ab84;
extern struct OamAttr data_ov006_0213ab8c;

#pragma define_section obj ".data.gap42" ".data.gap42" ".data.gap42" far_data
#pragma section obj begin
struct OamAttr * sFlowerFaceSprites[5] = {
    (struct OamAttr *)&data_ov006_0213ab8c,
    (struct OamAttr *)&data_ov006_0213ab84,
    (struct OamAttr *)&data_ov006_0213ab7c,
    (struct OamAttr *)&data_ov006_0213ab74,
    (struct OamAttr *)&data_ov006_0213ab6c,
};
#pragma section obj end
