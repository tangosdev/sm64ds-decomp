#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

extern u32 data_ov022_02113318[];
extern u32 data_ov022_02113360[];

u32 data_ov022_02113160[2] = {
    0x00000009,
    (u32)data_ov022_02113318,
};

u32 data_ov022_02113158[2] = {
    0x0000ec3d,
    0x00000000,
};

u32 data_ov022_02113150[2] = {
    0x0000ea3d,
    0x00000000,
};

u32 data_ov022_02113148[2] = {
    0x00000009,
    (u32)data_ov022_02113360,
};

u32 data_ov022_02113168[2] = {
    0x61676f79,
    0x0030306e,
};
