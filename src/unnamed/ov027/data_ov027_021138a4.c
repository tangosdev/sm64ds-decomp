#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} _ZTS7daOts_c_Layout;
typedef char _ZTS7daOts_c_size[sizeof(_ZTS7daOts_c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov027_021138a8_Layout;
typedef char data_ov027_021138a8_size[sizeof(data_ov027_021138a8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
} _ZTI12daIDonketu_c_Layout;
typedef char _ZTI12daIDonketu_c_size[sizeof(_ZTI12daIDonketu_c_Layout) == 0xc ? 1 : -1];

extern _ZTS7daOts_c_Layout _ZTS7daOts_c;
extern data_ov027_021138a8_Layout data_ov027_021138a8;
extern _ZTI12daIDonketu_c_Layout _ZTI12daIDonketu_c;
extern u8 _ZTS12daIDonketu_c[];
extern u8 _ZTI7daOts_c[];

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

_ZTS7daOts_c_Layout _ZTS7daOts_c = {{
    0x4f616437,
}};

data_ov027_021138a8_Layout data_ov027_021138a8 = {{
    0x635f7374,
    0x00000000,
}};

_ZTI12daIDonketu_c_Layout _ZTI12daIDonketu_c = {
    0x0209a764,
    &_ZTS12daIDonketu_c[0],
    &_ZTI7daOts_c[0],
};
