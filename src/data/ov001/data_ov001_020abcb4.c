/* Pointers, 9 entries. */
#include "types.h"

extern struct OamAttr data_ov001_020abc34;
extern struct OamAttr data_ov001_020abc3c;
extern struct OamAttr data_ov001_020abc44;
extern struct OamAttr data_ov001_020abc4c[];
extern struct OamAttr data_ov001_020abc64[];
extern struct OamAttr data_ov001_020abc7c[];
extern struct OamAttr data_ov001_020abc94[];
extern struct OamAttr data_ov001_020abcd8[];
extern struct OamAttr data_ov001_020abd18[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr * data_ov001_020abcb4[9] = {
    (struct OamAttr *)&data_ov001_020abc4c,
    (struct OamAttr *)&data_ov001_020abc64,
    (struct OamAttr *)&data_ov001_020abc7c,
    (struct OamAttr *)&data_ov001_020abc44,
    (struct OamAttr *)&data_ov001_020abc34,
    (struct OamAttr *)&data_ov001_020abc3c,
    (struct OamAttr *)&data_ov001_020abc94,
    (struct OamAttr *)&data_ov001_020abcd8,
    (struct OamAttr *)&data_ov001_020abd18,
};
#pragma section obj end
