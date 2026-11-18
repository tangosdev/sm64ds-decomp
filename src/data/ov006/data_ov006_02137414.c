/* Pointers, 7 entries. */
#include "types.h"

extern struct OamAttr data_ov006_021370b4;
extern struct OamAttr data_ov006_021370bc;
extern struct OamAttr data_ov006_021370cc;
extern struct OamAttr data_ov006_021370d4;
extern struct OamAttr data_ov006_021370dc;
extern struct OamAttr data_ov006_021370e4;
extern struct OamAttr data_ov006_021372b4[];

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr * data_ov006_02137414[7] = {
    (struct OamAttr *)&data_ov006_021370e4,
    (struct OamAttr *)&data_ov006_021370dc,
    (struct OamAttr *)&data_ov006_021370d4,
    (struct OamAttr *)&data_ov006_021370cc,
    (struct OamAttr *)&data_ov006_021372b4,
    (struct OamAttr *)&data_ov006_021370bc,
    (struct OamAttr *)&data_ov006_021370b4,
};
#pragma section obj end
