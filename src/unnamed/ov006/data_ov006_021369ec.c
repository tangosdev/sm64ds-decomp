#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
} data_ov006_021369ec_Layout;
typedef char data_ov006_021369ec_size[sizeof(data_ov006_021369ec_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_021369f8_Layout;
typedef char data_ov006_021369f8_size[sizeof(data_ov006_021369f8_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02136a08_Layout;
typedef char data_ov006_02136a08_size[sizeof(data_ov006_02136a08_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02136a18_Layout;
typedef char data_ov006_02136a18_size[sizeof(data_ov006_02136a18_Layout) == 0x10 ? 1 : -1];

extern data_ov006_021369ec_Layout data_ov006_021369ec;
extern data_ov006_021369f8_Layout data_ov006_021369f8;
extern data_ov006_02136a08_Layout data_ov006_02136a08;
extern data_ov006_02136a18_Layout data_ov006_02136a18;

#pragma define_section sName ".data.gap21"                                                         \
                             ".data.gap21"                                                         \
                             ".data.gap21" far_data
#pragma section sName begin

data_ov006_021369ec_Layout data_ov006_021369ec = {
    &data_ov006_02136a08,
    &data_ov006_021369f8,
    &data_ov006_02136a18,
};

data_ov006_021369f8_Layout data_ov006_021369f8 = {{
    0x41f800f8,
    0x000011d6,
    0x81f040f8,
    0xffff2122,
}};

data_ov006_02136a08_Layout data_ov006_02136a08 = {{
    0x41f800f8,
    0x000011d6,
    0x81f040f8,
    0xffff0122,
}};

data_ov006_02136a18_Layout data_ov006_02136a18 = {{
    0x41f800f8,
    0x000011d6,
    0x81f040f8,
    0xffff1122,
}};
