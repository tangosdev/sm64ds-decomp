#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
} data_ov025_02113998_Layout;
typedef char data_ov025_02113998_size[sizeof(data_ov025_02113998_Layout) == 0x8 ? 1 : -1];

extern data_ov025_02113998_Layout data_ov025_02113998;
extern u8 _ZTS10daDpLift_c[];
extern u8 _ZTI10dBgActor_c[];

#pragma define_section sName ".data.dkkTail"                                                       \
                             ".data.dkkTail"                                                       \
                             ".data.dkkTail" far_data
#pragma section sName begin

data_ov025_02113998_Layout data_ov025_02113998 = {
    &_ZTS10daDpLift_c[0],
    &_ZTI10dBgActor_c[0],
};
