#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
} data_ov006_021369b0_Layout;
typedef char data_ov006_021369b0_size[sizeof(data_ov006_021369b0_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_021369bc_Layout;
typedef char data_ov006_021369bc_size[sizeof(data_ov006_021369bc_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_021369cc_Layout;
typedef char data_ov006_021369cc_size[sizeof(data_ov006_021369cc_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_021369dc_Layout;
typedef char data_ov006_021369dc_size[sizeof(data_ov006_021369dc_Layout) == 0x10 ? 1 : -1];

extern data_ov006_021369b0_Layout data_ov006_021369b0;
extern data_ov006_021369bc_Layout data_ov006_021369bc;
extern data_ov006_021369cc_Layout data_ov006_021369cc;
extern data_ov006_021369dc_Layout data_ov006_021369dc;

#pragma define_section sName ".data.gap21"                                                         \
                             ".data.gap21"                                                         \
                             ".data.gap21" far_data
#pragma section sName begin

data_ov006_021369b0_Layout data_ov006_021369b0 = {
    &data_ov006_021369cc,
    &data_ov006_021369bc,
    &data_ov006_021369dc,
};

data_ov006_021369bc_Layout data_ov006_021369bc = {{
    0x41f800f8,
    0x000011d4,
    0x81f040f8,
    0xffff2122,
}};

data_ov006_021369cc_Layout data_ov006_021369cc = {{
    0x41f800f8,
    0x000011d4,
    0x81f040f8,
    0xffff0122,
}};

data_ov006_021369dc_Layout data_ov006_021369dc = {{
    0x41f800f8,
    0x000011d4,
    0x81f040f8,
    0xffff1122,
}};
