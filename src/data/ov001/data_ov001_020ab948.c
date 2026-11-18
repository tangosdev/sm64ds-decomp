/* Pointers, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov001_020ab958[];
extern struct OamAttr data_ov001_020ab968[];
extern struct OamAttr data_ov001_020ab988[];
extern struct OamAttr data_ov001_020ab9a8[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr * _ZN3OAM10LIFE_ICONSE[4] = {
    (struct OamAttr *)&data_ov001_020ab988,
    (struct OamAttr *)&data_ov001_020ab9a8,
    (struct OamAttr *)&data_ov001_020ab968,
    (struct OamAttr *)&data_ov001_020ab958,
};
#pragma section obj end
