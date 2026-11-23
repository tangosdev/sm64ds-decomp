#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} sPachinkoBallSprites_Layout;
typedef char sPachinkoBallSprites_size[sizeof(sPachinkoBallSprites_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02136cd8_Layout;
typedef char data_ov006_02136cd8_size[sizeof(data_ov006_02136cd8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02136ce0_Layout;
typedef char data_ov006_02136ce0_size[sizeof(data_ov006_02136ce0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02136ce8_Layout;
typedef char data_ov006_02136ce8_size[sizeof(data_ov006_02136ce8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
} data_ov006_02136cf0_Layout;
typedef char data_ov006_02136cf0_size[sizeof(data_ov006_02136cf0_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02136cfc_Layout;
typedef char data_ov006_02136cfc_size[sizeof(data_ov006_02136cfc_Layout) == 0x10 ? 1 : -1];

extern sPachinkoBallSprites_Layout sPachinkoBallSprites;
extern data_ov006_02136cd8_Layout data_ov006_02136cd8;
extern data_ov006_02136ce0_Layout data_ov006_02136ce0;
extern data_ov006_02136ce8_Layout data_ov006_02136ce8;
extern data_ov006_02136cf0_Layout data_ov006_02136cf0;
extern data_ov006_02136cfc_Layout data_ov006_02136cfc;

#pragma define_section sName ".data.gap22"                                                         \
                             ".data.gap22"                                                         \
                             ".data.gap22" far_data
#pragma section sName begin

sPachinkoBallSprites_Layout sPachinkoBallSprites = {
    &data_ov006_02136cd8,
};

data_ov006_02136ce8_Layout data_ov006_02136ce8 = {{
    0x81f000f0,
    0xffff1000,
}};

data_ov006_02136ce0_Layout data_ov006_02136ce0 = {{
    0x41fc80f0,
    0xffff1006,
}};

data_ov006_02136cd8_Layout data_ov006_02136cd8 = {{
    0x81f000f8,
    0xffff1010,
}};

data_ov006_02136cf0_Layout data_ov006_02136cf0 = {
    &data_ov006_02136ce8,
    &data_ov006_02136cfc,
    &data_ov006_02136ce0,
};

data_ov006_02136cfc_Layout data_ov006_02136cfc = {{
    0x81f880f0,
    0x00001004,
    0x400580f0,
    0xffff1006,
}};
