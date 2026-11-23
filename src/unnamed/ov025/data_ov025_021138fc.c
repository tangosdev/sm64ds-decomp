#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    u32 f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    void *f014;
} data_ov025_021138fc_Layout;
typedef char data_ov025_021138fc_size[sizeof(data_ov025_021138fc_Layout) == 0x18 ? 1 : -1];

extern data_ov025_021138fc_Layout data_ov025_021138fc;
extern u8 _ZTI14daObjDpBrock_c[];

#pragma define_section sName ".data.dkkTail"                                                       \
                             ".data.dkkTail"                                                       \
                             ".data.dkkTail" far_data
#pragma section sName begin

data_ov025_021138fc_Layout data_ov025_021138fc = {
    0x000fa000, 0x001f4000, 0x01900000, 0x00c80000, 0x00000000, &_ZTI14daObjDpBrock_c[0],
};
