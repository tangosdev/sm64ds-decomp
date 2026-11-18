/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov001_020ac658[];
extern struct OamAttr data_ov001_020ac7e0[];
extern struct OamAttr data_ov001_020ac980[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr * data_ov001_020ac64c[3] = {
    (struct OamAttr *)&data_ov001_020ac658,
    (struct OamAttr *)&data_ov001_020ac7e0,
    (struct OamAttr *)&data_ov001_020ac980,
};
#pragma section obj end
