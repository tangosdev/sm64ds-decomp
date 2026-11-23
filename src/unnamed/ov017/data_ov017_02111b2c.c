#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov017_02111b2c_Layout;
typedef char data_ov017_02111b2c_size[sizeof(data_ov017_02111b2c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov017_02111b34_Layout;
typedef char data_ov017_02111b34_size[sizeof(data_ov017_02111b34_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov017_02111b44_Layout;
typedef char data_ov017_02111b44_size[sizeof(data_ov017_02111b44_Layout) == 0x20 ? 1 : -1];

extern data_ov017_02111b2c_Layout data_ov017_02111b2c;
extern data_ov017_02111b34_Layout data_ov017_02111b34;
extern data_ov017_02111b44_Layout data_ov017_02111b44;

#pragma define_section sName ".data.ksRest"                                                        \
                             ".data.ksRest"                                                        \
                             ".data.ksRest" far_data
#pragma section sName begin

data_ov017_02111b2c_Layout data_ov017_02111b2c = {{
    0x03840000,
    0xfa880374,
}};

data_ov017_02111b34_Layout data_ov017_02111b34 = {{
    0x80000000,
    0x00800000,
    0x028a0000,
    0xfa880374,
}};

data_ov017_02111b44_Layout data_ov017_02111b44 = {{
    0x80000000,
    0x00800000,
    0x047e0000,
    0xfa880374,
    0x80000000,
    0x00800000,
    0x05460000,
    0xfa880374,
}};
