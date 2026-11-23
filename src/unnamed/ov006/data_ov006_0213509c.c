#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
} data_ov006_0213509c_Layout;
typedef char data_ov006_0213509c_size[sizeof(data_ov006_0213509c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_021350a8_Layout;
typedef char data_ov006_021350a8_size[sizeof(data_ov006_021350a8_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_021350b8_Layout;
typedef char data_ov006_021350b8_size[sizeof(data_ov006_021350b8_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_021350c8_Layout;
typedef char data_ov006_021350c8_size[sizeof(data_ov006_021350c8_Layout) == 0x10 ? 1 : -1];

extern data_ov006_0213509c_Layout data_ov006_0213509c;
extern data_ov006_021350a8_Layout data_ov006_021350a8;
extern data_ov006_021350b8_Layout data_ov006_021350b8;
extern data_ov006_021350c8_Layout data_ov006_021350c8;

#pragma define_section sName ".data.gap18"                                                         \
                             ".data.gap18"                                                         \
                             ".data.gap18" far_data
#pragma section sName begin

data_ov006_0213509c_Layout data_ov006_0213509c = {
    &data_ov006_021350b8,
    &data_ov006_021350a8,
    &data_ov006_021350c8,
};

data_ov006_021350a8_Layout data_ov006_021350a8 = {{
    0x41f800f8,
    0x000010c4,
    0x81f040f8,
    0xffff2122,
}};

data_ov006_021350b8_Layout data_ov006_021350b8 = {{
    0x41f800f8,
    0x000010c4,
    0x81f040f8,
    0xffff0122,
}};

data_ov006_021350c8_Layout data_ov006_021350c8 = {{
    0x41f800f8,
    0x000010c4,
    0x81f040f8,
    0xffff1122,
}};
