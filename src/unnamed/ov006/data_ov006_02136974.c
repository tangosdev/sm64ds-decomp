#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
} data_ov006_02136974_Layout;
typedef char data_ov006_02136974_size[sizeof(data_ov006_02136974_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02136980_Layout;
typedef char data_ov006_02136980_size[sizeof(data_ov006_02136980_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02136990_Layout;
typedef char data_ov006_02136990_size[sizeof(data_ov006_02136990_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_021369a0_Layout;
typedef char data_ov006_021369a0_size[sizeof(data_ov006_021369a0_Layout) == 0x10 ? 1 : -1];

extern data_ov006_02136974_Layout data_ov006_02136974;
extern data_ov006_02136980_Layout data_ov006_02136980;
extern data_ov006_02136990_Layout data_ov006_02136990;
extern data_ov006_021369a0_Layout data_ov006_021369a0;

#pragma define_section sName ".data.gap21"                                                         \
                             ".data.gap21"                                                         \
                             ".data.gap21" far_data
#pragma section sName begin

data_ov006_02136974_Layout data_ov006_02136974 = {
    &data_ov006_02136990,
    &data_ov006_02136980,
    &data_ov006_021369a0,
};

data_ov006_02136980_Layout data_ov006_02136980 = {{
    0x41f800f8,
    0x000010c0,
    0x81f040f8,
    0xffff2122,
}};

data_ov006_02136990_Layout data_ov006_02136990 = {{
    0x41f800f8,
    0x000010c0,
    0x81f040f8,
    0xffff0122,
}};

data_ov006_021369a0_Layout data_ov006_021369a0 = {{
    0x41f800f8,
    0x000010c0,
    0x81f040f8,
    0xffff1122,
}};
