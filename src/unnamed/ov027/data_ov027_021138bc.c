#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
} _ZTI7daOts_c_Layout;
typedef char _ZTI7daOts_c_size[sizeof(_ZTI7daOts_c_Layout) == 0x8 ? 1 : -1];

extern _ZTI7daOts_c_Layout _ZTI7daOts_c;
extern u8 _ZTS7daOts_c[];

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

_ZTI7daOts_c_Layout _ZTI7daOts_c = {
    0x0209a764,
    &_ZTS7daOts_c[0],
};
