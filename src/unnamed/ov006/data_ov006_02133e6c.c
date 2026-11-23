#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov006_02133e6c_Layout;
typedef char data_ov006_02133e6c_size[sizeof(data_ov006_02133e6c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02133e74_Layout;
typedef char data_ov006_02133e74_size[sizeof(data_ov006_02133e74_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
    void *f010;
} sPachinkoFxSpritesB_Layout;
typedef char sPachinkoFxSpritesB_size[sizeof(sPachinkoFxSpritesB_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_02133e90_Layout;
typedef char data_ov006_02133e90_size[sizeof(data_ov006_02133e90_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_02133eb0_Layout;
typedef char data_ov006_02133eb0_size[sizeof(data_ov006_02133eb0_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov006_02133ed0_Layout;
typedef char data_ov006_02133ed0_size[sizeof(data_ov006_02133ed0_Layout) == 0x30 ? 1 : -1];

extern data_ov006_02133e6c_Layout data_ov006_02133e6c;
extern data_ov006_02133e74_Layout data_ov006_02133e74;
extern sPachinkoFxSpritesB_Layout sPachinkoFxSpritesB;
extern data_ov006_02133e90_Layout data_ov006_02133e90;
extern data_ov006_02133eb0_Layout data_ov006_02133eb0;
extern data_ov006_02133ed0_Layout data_ov006_02133ed0;

#pragma define_section sName ".data.gap4"                                                          \
                             ".data.gap4"                                                          \
                             ".data.gap4" far_data
#pragma section sName begin

data_ov006_02133e6c_Layout data_ov006_02133e6c = {{
    0x41f800f9,
    0xffff5140,
}};

data_ov006_02133e74_Layout data_ov006_02133e74 = {{
    0x81f000f0,
    0xffff5180,
}};

sPachinkoFxSpritesB_Layout sPachinkoFxSpritesB = {
    &data_ov006_02133e6c, &data_ov006_02133e90, &data_ov006_02133e74,
    &data_ov006_02133ed0, &data_ov006_02133eb0,
};

data_ov006_02133e90_Layout data_ov006_02133e90 = {{
    0x41f000f0,
    0x00005142,
    0x61f00000,
    0x00005142,
    0x500000f0,
    0x00005142,
    0x70000000,
    0xffff5142,
}};

data_ov006_02133eb0_Layout data_ov006_02133eb0 = {{
    0x01f000f0,
    0x00005185,
    0x000000f0,
    0x00005166,
    0x000800f0,
    0x00005186,
    0x00080008,
    0xffff51a7,
}};

data_ov006_02133ed0_Layout data_ov006_02133ed0 = {{
    0x01f080f0,
    0x00005144,
    0x01f08000,
    0x00005184,
    0x01f800f0,
    0x00005145,
    0x000040f0,
    0x00005146,
    0x000800f8,
    0x00005165,
    0x00004008,
    0xffff51a5,
}};
