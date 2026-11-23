#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov006_0213a5b8_Layout;
typedef char data_ov006_0213a5b8_size[sizeof(data_ov006_0213a5b8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0213a5c0_Layout;
typedef char data_ov006_0213a5c0_size[sizeof(data_ov006_0213a5c0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0213a5c8_Layout;
typedef char data_ov006_0213a5c8_size[sizeof(data_ov006_0213a5c8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0213a5d0_Layout;
typedef char data_ov006_0213a5d0_size[sizeof(data_ov006_0213a5d0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0213a5d8_Layout;
typedef char data_ov006_0213a5d8_size[sizeof(data_ov006_0213a5d8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
    void *f010;
} sCurlingBitSprites_Layout;
typedef char sCurlingBitSprites_size[sizeof(sCurlingBitSprites_Layout) == 0x14 ? 1 : -1];

extern data_ov006_0213a5b8_Layout data_ov006_0213a5b8;
extern data_ov006_0213a5c0_Layout data_ov006_0213a5c0;
extern data_ov006_0213a5c8_Layout data_ov006_0213a5c8;
extern data_ov006_0213a5d0_Layout data_ov006_0213a5d0;
extern data_ov006_0213a5d8_Layout data_ov006_0213a5d8;
extern sCurlingBitSprites_Layout sCurlingBitSprites;

#pragma define_section sName ".data.gap38"                                                         \
                             ".data.gap38"                                                         \
                             ".data.gap38" far_data
#pragma section sName begin

data_ov006_0213a5d8_Layout data_ov006_0213a5d8 = {{
    0x01fc00fc,
    0xffff2048,
}};

data_ov006_0213a5d0_Layout data_ov006_0213a5d0 = {{
    0x01fc00fc,
    0xffff2049,
}};

data_ov006_0213a5c8_Layout data_ov006_0213a5c8 = {{
    0x01fc00fc,
    0xffff204a,
}};

data_ov006_0213a5c0_Layout data_ov006_0213a5c0 = {{
    0x01fc00fc,
    0xffff204b,
}};

data_ov006_0213a5b8_Layout data_ov006_0213a5b8 = {{
    0x01fc00fc,
    0xffff204c,
}};

sCurlingBitSprites_Layout sCurlingBitSprites = {
    &data_ov006_0213a5d8, &data_ov006_0213a5d0, &data_ov006_0213a5c8,
    &data_ov006_0213a5c0, &data_ov006_0213a5b8,
};
