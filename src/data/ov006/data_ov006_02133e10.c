/* Word table, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02133e08;
extern struct OamAttr data_ov006_02133e1c[];
extern struct OamAttr data_ov006_02133e3c[];

#pragma define_section obj ".data.gap4" ".data.gap4" ".data.gap4" far_data
#pragma section obj begin
int sPachinkoFxSpritesA[3] = {
    (int)&data_ov006_02133e08,
    (int)&data_ov006_02133e3c,
    (int)&data_ov006_02133e1c,
};
#pragma section obj end
