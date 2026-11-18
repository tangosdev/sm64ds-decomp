/* Word table, 5 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02133e6c;
extern struct OamAttr data_ov006_02133e74;
extern struct OamAttr data_ov006_02133e90[];
extern struct OamAttr data_ov006_02133eb0[];
extern struct OamAttr data_ov006_02133ed0[];

#pragma define_section obj ".data.gap4" ".data.gap4" ".data.gap4" far_data
#pragma section obj begin
int sPachinkoFxSpritesB[5] = {
    (int)&data_ov006_02133e6c,
    (int)&data_ov006_02133e90,
    (int)&data_ov006_02133e74,
    (int)&data_ov006_02133ed0,
    (int)&data_ov006_02133eb0,
};
#pragma section obj end
