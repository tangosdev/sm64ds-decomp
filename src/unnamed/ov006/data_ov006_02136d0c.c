#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov006_02136d0c_Layout;
typedef char data_ov006_02136d0c_size[sizeof(data_ov006_02136d0c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02136d14_Layout;
typedef char data_ov006_02136d14_size[sizeof(data_ov006_02136d14_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
} data_ov006_02136d1c_Layout;
typedef char data_ov006_02136d1c_size[sizeof(data_ov006_02136d1c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02136d28_Layout;
typedef char data_ov006_02136d28_size[sizeof(data_ov006_02136d28_Layout) == 0x10 ? 1 : -1];

extern data_ov006_02136d0c_Layout data_ov006_02136d0c;
extern data_ov006_02136d14_Layout data_ov006_02136d14;
extern data_ov006_02136d1c_Layout data_ov006_02136d1c;
extern data_ov006_02136d28_Layout data_ov006_02136d28;

#pragma define_section sName ".data.gap22"                                                         \
                             ".data.gap22"                                                         \
                             ".data.gap22" far_data
#pragma section sName begin

data_ov006_02136d0c_Layout data_ov006_02136d0c = {{
    0x41fc80f0,
    0xffff1006,
}};

data_ov006_02136d14_Layout data_ov006_02136d14 = {{
    0x81f000f0,
    0xffff1007,
}};

data_ov006_02136d1c_Layout data_ov006_02136d1c = {
    &data_ov006_02136d14,
    &data_ov006_02136d28,
    &data_ov006_02136d0c,
};

data_ov006_02136d28_Layout data_ov006_02136d28 = {{
    0x81f880f0,
    0x0000100b,
    0x400580f0,
    0xffff1006,
}};
