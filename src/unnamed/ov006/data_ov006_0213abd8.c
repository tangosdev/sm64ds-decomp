#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov006_0213abd8_Layout;
typedef char data_ov006_0213abd8_size[sizeof(data_ov006_0213abd8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
} data_ov006_0213abe0_Layout;
typedef char data_ov006_0213abe0_size[sizeof(data_ov006_0213abe0_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_0213abec_Layout;
typedef char data_ov006_0213abec_size[sizeof(data_ov006_0213abec_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_0213ac04_Layout;
typedef char data_ov006_0213ac04_size[sizeof(data_ov006_0213ac04_Layout) == 0x20 ? 1 : -1];

extern data_ov006_0213abd8_Layout data_ov006_0213abd8;
extern data_ov006_0213abe0_Layout data_ov006_0213abe0;
extern data_ov006_0213abec_Layout data_ov006_0213abec;
extern data_ov006_0213ac04_Layout data_ov006_0213ac04;

#pragma define_section sName ".data.gap43"                                                         \
                             ".data.gap43"                                                         \
                             ".data.gap43" far_data
#pragma section sName begin

data_ov006_0213abd8_Layout data_ov006_0213abd8 = {{
    0xc1e000e0,
    0xffff0000,
}};

data_ov006_0213abe0_Layout data_ov006_0213abe0 = {
    &data_ov006_0213abd8,
    &data_ov006_0213abec,
    &data_ov006_0213ac04,
};

data_ov006_0213abec_Layout data_ov006_0213abec = {{
    0x81f080e8,
    0x00000022,
    0x41f00008,
    0x000000a2,
    0x01f800e0,
    0xffff0003,
}};

data_ov006_0213ac04_Layout data_ov006_0213ac04 = {{
    0x800080e8,
    0x00000024,
    0x40000008,
    0x000000a4,
    0x000000e0,
    0x00000004,
    0x001080f8,
    0xffff0066,
}};
