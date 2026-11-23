#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} data_ov006_02136d98_Layout;
typedef char data_ov006_02136d98_size[sizeof(data_ov006_02136d98_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02136da8_Layout;
typedef char data_ov006_02136da8_size[sizeof(data_ov006_02136da8_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02136db8_Layout;
typedef char data_ov006_02136db8_size[sizeof(data_ov006_02136db8_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02136dc8_Layout;
typedef char data_ov006_02136dc8_size[sizeof(data_ov006_02136dc8_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02136dd8_Layout;
typedef char data_ov006_02136dd8_size[sizeof(data_ov006_02136dd8_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
    void *f010;
} data_ov006_02136de8_Layout;
typedef char data_ov006_02136de8_size[sizeof(data_ov006_02136de8_Layout) == 0x14 ? 1 : -1];

extern data_ov006_02136d98_Layout data_ov006_02136d98;
extern data_ov006_02136da8_Layout data_ov006_02136da8;
extern data_ov006_02136db8_Layout data_ov006_02136db8;
extern data_ov006_02136dc8_Layout data_ov006_02136dc8;
extern data_ov006_02136dd8_Layout data_ov006_02136dd8;
extern data_ov006_02136de8_Layout data_ov006_02136de8;

#pragma define_section sName ".data.gap22"                                                         \
                             ".data.gap22"                                                         \
                             ".data.gap22" far_data
#pragma section sName begin

data_ov006_02136dd8_Layout data_ov006_02136dd8 = {{
    0x41f800f0,
    0x0000000d,
    0xc1f080e0,
    0xffff001c,
}};

data_ov006_02136dc8_Layout data_ov006_02136dc8 = {{
    0x01fc80f0,
    0x0000000f,
    0xc1f080e0,
    0xffff001c,
}};

data_ov006_02136db8_Layout data_ov006_02136db8 = {{
    0x41f800f0,
    0x00000010,
    0xc1f080e0,
    0xffff001c,
}};

data_ov006_02136da8_Layout data_ov006_02136da8 = {{
    0x41f800f0,
    0x00000012,
    0xc1f080e0,
    0xffff001c,
}};

data_ov006_02136d98_Layout data_ov006_02136d98 = {{
    0x41f800f0,
    0x00000014,
    0xc1f080e0,
    0xffff001c,
}};

data_ov006_02136de8_Layout data_ov006_02136de8 = {
    &data_ov006_02136dd8, &data_ov006_02136dc8, &data_ov006_02136db8,
    &data_ov006_02136da8, &data_ov006_02136d98,
};
