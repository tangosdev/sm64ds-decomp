/* Pointers, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov001_020abda8[];
extern struct OamAttr data_ov001_020abdd8[];
extern struct OamAttr data_ov001_020abf20[];
extern struct OamAttr data_ov001_020ac078[];

struct OamAttr *data_ov001_020abd98[4] = {
    (struct OamAttr *)&data_ov001_020abf20,
    (struct OamAttr *)&data_ov001_020abdd8,
    (struct OamAttr *)&data_ov001_020ac078,
    (struct OamAttr *)&data_ov001_020abda8,
};
