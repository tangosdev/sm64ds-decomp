/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov001_020ac224[];
extern struct OamAttr data_ov001_020ac35c[];
extern struct OamAttr data_ov001_020ac4ac[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr * data_ov001_020ac218[3] = {
    (struct OamAttr *)&data_ov001_020ac35c,
    (struct OamAttr *)&data_ov001_020ac224,
    (struct OamAttr *)&data_ov001_020ac4ac,
};
#pragma section obj end
