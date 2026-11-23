#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} data_ov025_021138d8_Layout;
typedef char data_ov025_021138d8_size[sizeof(data_ov025_021138d8_Layout) == 0x4 ? 1 : -1];

extern data_ov025_021138d8_Layout data_ov025_021138d8;
extern u8 _ZTI10dBgActor_c[];

#pragma define_section sName ".data.dkkTail"                                                       \
                             ".data.dkkTail"                                                       \
                             ".data.dkkTail" far_data
#pragma section sName begin

data_ov025_021138d8_Layout data_ov025_021138d8 = {
    &_ZTI10dBgActor_c[0],
};
