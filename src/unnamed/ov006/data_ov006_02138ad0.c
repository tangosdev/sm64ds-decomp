#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov006_02138ad0_Layout;
typedef char data_ov006_02138ad0_size[sizeof(data_ov006_02138ad0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02138ad8_Layout;
typedef char data_ov006_02138ad8_size[sizeof(data_ov006_02138ad8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
    void *f010;
    void *f014;
} data_ov006_02138ae0_Layout;
typedef char data_ov006_02138ae0_size[sizeof(data_ov006_02138ae0_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov006_02138af8_Layout;
typedef char data_ov006_02138af8_size[sizeof(data_ov006_02138af8_Layout) == 0x30 ? 1 : -1];

typedef struct {
    u32 w[14];
} data_ov006_02138b28_Layout;
typedef char data_ov006_02138b28_size[sizeof(data_ov006_02138b28_Layout) == 0x38 ? 1 : -1];

typedef struct {
    u32 w[14];
} data_ov006_02138b60_Layout;
typedef char data_ov006_02138b60_size[sizeof(data_ov006_02138b60_Layout) == 0x38 ? 1 : -1];

typedef struct {
    u32 w[14];
} data_ov006_02138b98_Layout;
typedef char data_ov006_02138b98_size[sizeof(data_ov006_02138b98_Layout) == 0x38 ? 1 : -1];

extern data_ov006_02138ad0_Layout data_ov006_02138ad0;
extern data_ov006_02138ad8_Layout data_ov006_02138ad8;
extern data_ov006_02138ae0_Layout data_ov006_02138ae0;
extern data_ov006_02138af8_Layout data_ov006_02138af8;
extern data_ov006_02138b28_Layout data_ov006_02138b28;
extern data_ov006_02138b60_Layout data_ov006_02138b60;
extern data_ov006_02138b98_Layout data_ov006_02138b98;

#pragma define_section sName ".data.gap32"                                                         \
                             ".data.gap32"                                                         \
                             ".data.gap32" far_data
#pragma section sName begin

data_ov006_02138ad0_Layout data_ov006_02138ad0 = {{
    0x81f000f0,
    0xffff7004,
}};

data_ov006_02138ad8_Layout data_ov006_02138ad8 = {{
    0x41f800f8,
    0xffff71da,
}};

data_ov006_02138ae0_Layout data_ov006_02138ae0 = {
    &data_ov006_02138ad8, &data_ov006_02138af8, &data_ov006_02138b60,
    &data_ov006_02138b98, &data_ov006_02138b28, &data_ov006_02138ad0,
};

data_ov006_02138af8_Layout data_ov006_02138af8 = {{
    0x01e840fd,
    0x000061fc,
    0x01f840fd,
    0x000061fd,
    0x000840fd,
    0x000061fe,
    0x41ed00f8,
    0x000071da,
    0x41f800f8,
    0x000071da,
    0x400300f8,
    0xffff71da,
}};

data_ov006_02138b28_Layout data_ov006_02138b28 = {{
    0x41f800f8,
    0x0000719e,
    0x01e840fd,
    0x000061fc,
    0x01f840fd,
    0x000061fd,
    0x000840fd,
    0x000061fe,
    0x41ed00f8,
    0x000071da,
    0x41f800f8,
    0x000071da,
    0x400300f8,
    0xffff71da,
}};

data_ov006_02138b60_Layout data_ov006_02138b60 = {{
    0x41f800f8,
    0x0000719a,
    0x01e840fd,
    0x000061fc,
    0x01f840fd,
    0x000061fd,
    0x000840fd,
    0x000061fe,
    0x41ed00f8,
    0x000071da,
    0x41f800f8,
    0x000071da,
    0x400300f8,
    0xffff71da,
}};

data_ov006_02138b98_Layout data_ov006_02138b98 = {{
    0x41f800f8,
    0x0000719c,
    0x01e840fd,
    0x000061fc,
    0x01f840fd,
    0x000061fd,
    0x000840fd,
    0x000061fe,
    0x41ed00f8,
    0x000071da,
    0x41f800f8,
    0x000071da,
    0x400300f8,
    0xffff71da,
}};
