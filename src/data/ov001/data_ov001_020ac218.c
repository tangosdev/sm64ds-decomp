/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr data_ov001_020ac224[];
extern struct OamAttr data_ov001_020ac35c[];
extern struct OamAttr data_ov001_020ac4ac[];

struct OamAttr *data_ov001_020ac218[3] = {
    (struct OamAttr *)&data_ov001_020ac35c,
    (struct OamAttr *)&data_ov001_020ac224,
    (struct OamAttr *)&data_ov001_020ac4ac,
};
