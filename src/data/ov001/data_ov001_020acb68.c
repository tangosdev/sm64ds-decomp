/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov001_020acb74[];
extern struct OamAttr data_ov001_020accfc[];
extern struct OamAttr data_ov001_020ace84[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr * data_ov001_020acb68[3] = {
    (struct OamAttr *)&data_ov001_020accfc,
    (struct OamAttr *)&data_ov001_020acb74,
    (struct OamAttr *)&data_ov001_020ace84,
};
#pragma section obj end
