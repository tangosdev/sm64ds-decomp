#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov018_02112e80_Layout;
typedef char data_ov018_02112e80_size[sizeof(data_ov018_02112e80_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov018_02112e84_Layout;
typedef char data_ov018_02112e84_size[sizeof(data_ov018_02112e84_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov018_02112e88_Layout;
typedef char data_ov018_02112e88_size[sizeof(data_ov018_02112e88_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov018_02112e90_Layout;
typedef char data_ov018_02112e90_size[sizeof(data_ov018_02112e90_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov018_02112e98_Layout;
typedef char data_ov018_02112e98_size[sizeof(data_ov018_02112e98_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov018_02112ea0_Layout;
typedef char data_ov018_02112ea0_size[sizeof(data_ov018_02112ea0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov018_02112ea8_Layout;
typedef char data_ov018_02112ea8_size[sizeof(data_ov018_02112ea8_Layout) == 0xc ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
} data_ov018_02112eb4_Layout;
typedef char data_ov018_02112eb4_size[sizeof(data_ov018_02112eb4_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov018_02112ec0_Layout;
typedef char data_ov018_02112ec0_size[sizeof(data_ov018_02112ec0_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov018_02112ecc_Layout;
typedef char data_ov018_02112ecc_size[sizeof(data_ov018_02112ecc_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov018_02112edc_Layout;
typedef char data_ov018_02112edc_size[sizeof(data_ov018_02112edc_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov018_02112eec_Layout;
typedef char data_ov018_02112eec_size[sizeof(data_ov018_02112eec_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
    void *f00c;
    u32 f010;
    void *f014;
} data_ov018_02112efc_Layout;
typedef char data_ov018_02112efc_size[sizeof(data_ov018_02112efc_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov018_02112f14_Layout;
typedef char data_ov018_02112f14_size[sizeof(data_ov018_02112f14_Layout) == 0x18 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    u32 f00c;
    void *f010;
    u32 f014;
    u32 f018;
} data_ov018_02112f2c_Layout;
typedef char data_ov018_02112f2c_size[sizeof(data_ov018_02112f2c_Layout) == 0x1c ? 1 : -1];

extern data_ov018_02112e80_Layout data_ov018_02112e80;
extern data_ov018_02112e84_Layout data_ov018_02112e84;
extern data_ov018_02112e88_Layout data_ov018_02112e88;
extern data_ov018_02112e90_Layout data_ov018_02112e90;
extern data_ov018_02112e98_Layout data_ov018_02112e98;
extern data_ov018_02112ea0_Layout data_ov018_02112ea0;
extern data_ov018_02112ea8_Layout data_ov018_02112ea8;
extern data_ov018_02112eb4_Layout data_ov018_02112eb4;
extern data_ov018_02112ec0_Layout data_ov018_02112ec0;
extern data_ov018_02112ecc_Layout data_ov018_02112ecc;
extern data_ov018_02112edc_Layout data_ov018_02112edc;
extern data_ov018_02112eec_Layout data_ov018_02112eec;
extern data_ov018_02112efc_Layout data_ov018_02112efc;
extern data_ov018_02112f14_Layout data_ov018_02112f14;
extern data_ov018_02112f2c_Layout data_ov018_02112f2c;
extern u8 data_ov018_02113120[];
extern u8 data_ov018_02113170[];
extern u8 data_ov018_02112a78[];
extern u8 data_ov018_02112a74[];
extern u8 data_ov018_02112a98[];
extern u8 data_ov018_02112a7c[];
extern u8 data_ov018_02113270[];

data_ov018_02112e80_Layout data_ov018_02112e80 = {{
    0x000007be,
}};

data_ov018_02112e84_Layout data_ov018_02112e84 = {{
    0x00100114,
}};

data_ov018_02112e90_Layout data_ov018_02112e90 = {
    0x0000000a,
    &data_ov018_02113120[0],
};

data_ov018_02112e88_Layout data_ov018_02112e88 = {{
    0x0992423f,
    0x1612eed0,
}};

data_ov018_02112ea0_Layout data_ov018_02112ea0 = {
    0x0000000b,
    &data_ov018_02113170[0],
};

data_ov018_02112e98_Layout data_ov018_02112e98 = {{
    0xee6c4a3f,
    0x0672faed,
}};

data_ov018_02112eb4_Layout data_ov018_02112eb4 = {
    &data_ov018_02112ea0,
    &data_ov018_02112efc,
    0x157c0000,
};

data_ov018_02112ec0_Layout data_ov018_02112ec0 = {{
    0xee6c0990,
    0xa0001027,
    0x00080000,
}};

data_ov018_02112ea8_Layout data_ov018_02112ea8 = {{
    0x5f74616d,
    0x65746177,
    0x00000072,
}};

data_ov018_02112ecc_Layout data_ov018_02112ecc = {{
    0x0b86ff4f,
    0xc000faba,
    0x000b0000,
    0x000011ff,
}};

data_ov018_02112edc_Layout data_ov018_02112edc = {{
    0x0acaf8c6,
    0x0002fe84,
    0xee080c94,
    0x1002ff6a,
}};

data_ov018_02112eec_Layout data_ov018_02112eec = {{
    0xee180c80,
    0x0040ff6a,
    0x0acaf8c6,
    0x00c0fe70,
}};

data_ov018_02112efc_Layout data_ov018_02112efc = {
    0x0000005b, &data_ov018_02112a78[0], &data_ov018_02112a74[0], &data_ov018_02112a98[0],
    0x00000001, &data_ov018_02112a7c[0],
};

data_ov018_02112f14_Layout data_ov018_02112f14 = {{
    0xff020000,
    0x00027fff,
    0x7fffff05,
    0xff130007,
    0x001a7fff,
    0x7fffff02,
}};

data_ov018_02112f2c_Layout data_ov018_02112f2c = {
    &data_ov018_02113270[0], &data_ov018_02112e90, 0x07bd07bf, 0x07c107c0,
    &data_ov018_02112eb4,    0x05780001,           0x0000004f,
};
