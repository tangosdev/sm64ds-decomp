/* Pointers, 7 entries. */
#include "types.h"

extern struct OamAttr data_ov001_020abae0;
extern struct OamAttr data_ov001_020abae8;
extern struct OamAttr data_ov001_020abaf0;
extern struct OamAttr data_ov001_020abaf8;
extern struct OamAttr data_ov001_020abb00;
extern struct OamAttr data_ov001_020abb08;
extern struct OamAttr data_ov001_020abb10;

struct OamAttr *data_ov001_020abb18[7] = {
    (struct OamAttr *)&data_ov001_020abb10, (struct OamAttr *)&data_ov001_020abb08,
    (struct OamAttr *)&data_ov001_020abb00, (struct OamAttr *)&data_ov001_020abaf8,
    (struct OamAttr *)&data_ov001_020abaf0, (struct OamAttr *)&data_ov001_020abae8,
    (struct OamAttr *)&data_ov001_020abae0,
};
