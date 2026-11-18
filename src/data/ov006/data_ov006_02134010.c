/* Pointers, 6 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02133fb0[];
extern struct OamAttr data_ov006_02133fc0[];
extern struct OamAttr data_ov006_02133fd0[];
extern struct OamAttr data_ov006_02133fe0[];
extern struct OamAttr data_ov006_02133ff0[];
extern struct OamAttr data_ov006_02134000[];

#pragma define_section obj ".data.gap6" ".data.gap6" ".data.gap6" far_data
#pragma section obj begin
struct OamAttr * sCardPayoutRowSprites[6] = {
    (struct OamAttr *)&data_ov006_02134000,
    (struct OamAttr *)&data_ov006_02133ff0,
    (struct OamAttr *)&data_ov006_02133fe0,
    (struct OamAttr *)&data_ov006_02133fd0,
    (struct OamAttr *)&data_ov006_02133fc0,
    (struct OamAttr *)&data_ov006_02133fb0,
};
#pragma section obj end
