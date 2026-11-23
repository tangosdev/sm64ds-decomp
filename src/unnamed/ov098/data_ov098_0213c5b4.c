#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
} data_ov098_0213c5b4_Layout;
typedef char data_ov098_0213c5b4_size[sizeof(data_ov098_0213c5b4_Layout) == 0x8 ? 1 : -1];

extern data_ov098_0213c5b4_Layout data_ov098_0213c5b4;
extern u8 _ZTI16daObjFallBlock_c[];

#pragma define_section sName ".data.blockTail"                                                     \
                             ".data.blockTail"                                                     \
                             ".data.blockTail" far_data
#pragma section sName begin

data_ov098_0213c5b4_Layout data_ov098_0213c5b4 = {
    0x00000000,
    &_ZTI16daObjFallBlock_c[0],
};
