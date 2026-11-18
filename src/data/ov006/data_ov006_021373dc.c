/* Pointers, 7 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02136e5c;
extern struct OamAttr data_ov006_02136e8c;
extern struct OamAttr data_ov006_02136e94;
extern struct OamAttr data_ov006_02136ea4;
extern struct OamAttr data_ov006_02136eac;
extern struct OamAttr data_ov006_02137124[];
extern struct OamAttr data_ov006_021371f4[];

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr * data_ov006_021373dc[7] = {
    (struct OamAttr *)&data_ov006_02136eac,
    (struct OamAttr *)&data_ov006_02136ea4,
    (struct OamAttr *)&data_ov006_02136e94,
    (struct OamAttr *)&data_ov006_02137124,
    (struct OamAttr *)&data_ov006_021371f4,
    (struct OamAttr *)&data_ov006_02136e8c,
    (struct OamAttr *)&data_ov006_02136e5c,
};
#pragma section obj end
