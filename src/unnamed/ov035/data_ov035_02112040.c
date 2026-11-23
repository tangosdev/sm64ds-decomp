#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov035_02112040_Layout;
typedef char data_ov035_02112040_size[sizeof(data_ov035_02112040_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov035_02112044_Layout;
typedef char data_ov035_02112044_size[sizeof(data_ov035_02112044_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov035_02112048_Layout;
typedef char data_ov035_02112048_size[sizeof(data_ov035_02112048_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov035_0211204c_Layout;
typedef char data_ov035_0211204c_size[sizeof(data_ov035_0211204c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov035_02112054_Layout;
typedef char data_ov035_02112054_size[sizeof(data_ov035_02112054_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov035_0211205c_Layout;
typedef char data_ov035_0211205c_size[sizeof(data_ov035_0211205c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov035_02112064_Layout;
typedef char data_ov035_02112064_size[sizeof(data_ov035_02112064_Layout) == 0xc ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
} data_ov035_02112070_Layout;
typedef char data_ov035_02112070_size[sizeof(data_ov035_02112070_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov035_0211207c_Layout;
typedef char data_ov035_0211207c_size[sizeof(data_ov035_0211207c_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
    void *f00c;
    u32 f010;
    void *f014;
} data_ov035_0211208c_Layout;
typedef char data_ov035_0211208c_size[sizeof(data_ov035_0211208c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
    void *f00c;
    u32 f010;
    void *f014;
} data_ov035_021120a4_Layout;
typedef char data_ov035_021120a4_size[sizeof(data_ov035_021120a4_Layout) == 0x18 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    u32 f00c;
    void *f010;
    u32 f014;
    u32 f018;
} data_ov035_021120bc_Layout;
typedef char data_ov035_021120bc_size[sizeof(data_ov035_021120bc_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[64];
} data_ov035_021120d8_Layout;
typedef char data_ov035_021120d8_size[sizeof(data_ov035_021120d8_Layout) == 0x100 ? 1 : -1];

extern data_ov035_02112040_Layout data_ov035_02112040;
extern data_ov035_02112044_Layout data_ov035_02112044;
extern data_ov035_02112048_Layout data_ov035_02112048;
extern data_ov035_0211204c_Layout data_ov035_0211204c;
extern data_ov035_02112054_Layout data_ov035_02112054;
extern data_ov035_0211205c_Layout data_ov035_0211205c;
extern data_ov035_02112064_Layout data_ov035_02112064;
extern data_ov035_02112070_Layout data_ov035_02112070;
extern data_ov035_0211207c_Layout data_ov035_0211207c;
extern data_ov035_0211208c_Layout data_ov035_0211208c;
extern data_ov035_021120a4_Layout data_ov035_021120a4;
extern data_ov035_021120bc_Layout data_ov035_021120bc;
extern data_ov035_021120d8_Layout data_ov035_021120d8;
extern u8 data_ov035_02112278[];
extern u8 data_ov035_02112218[];
extern u8 data_ov035_02111bdc[];
extern u8 data_ov035_02111bd4[];
extern u8 data_ov035_02111d84[];
extern u8 data_ov035_02111bfc[];
extern u8 data_ov035_02111bd8[];
extern u8 data_ov035_02111bd0[];
extern u8 data_ov035_02111c18[];
extern u8 data_ov035_02111be0[];
extern u8 data_ov035_0211236c[];

#pragma define_section sName ".data.m35Pre"                                                        \
                             ".data.m35Pre"                                                        \
                             ".data.m35Pre" far_data
#pragma section sName begin

data_ov035_02112044_Layout data_ov035_02112044 = {{
    0x0000070d,
}};

data_ov035_02112048_Layout data_ov035_02112048 = {{
    0x00000004,
}};

data_ov035_02112040_Layout data_ov035_02112040 = {{
    0x00000a8c,
}};

data_ov035_0211204c_Layout data_ov035_0211204c = {
    0x00000006,
    &data_ov035_02112278[0],
};

data_ov035_02112054_Layout data_ov035_02112054 = {
    0x00000004,
    &data_ov035_02112218[0],
};

data_ov035_0211205c_Layout data_ov035_0211205c = {{
    0xffffff01,
    0x75302710,
}};

data_ov035_02112064_Layout data_ov035_02112064 = {{
    0x6863656d,
    0x6f635f61,
    0x0000766e,
}};

data_ov035_02112070_Layout data_ov035_02112070 = {
    &data_ov035_02112054,
    0x00000000,
    0x1f400000,
};

data_ov035_0211207c_Layout data_ov035_0211207c = {{
    0x05dc0014,
    0x04b0ed9a,
    0xa0000003,
    0xffff0000,
}};

data_ov035_021120a4_Layout data_ov035_021120a4 = {
    0x0000005b, &data_ov035_02111bd8[0], &data_ov035_02111bd0[0], &data_ov035_02111c18[0],
    0x00000001, &data_ov035_02111be0[0],
};

data_ov035_0211208c_Layout data_ov035_0211208c = {
    0x0000005b, &data_ov035_02111bdc[0], &data_ov035_02111bd4[0], &data_ov035_02111d84[0],
    0x00000001, &data_ov035_02111bfc[0],
};

data_ov035_021120bc_Layout data_ov035_021120bc = {
    &data_ov035_0211236c[0], &data_ov035_0211204c, 0x070c070e, 0x0710070f,
    &data_ov035_02112070,    0x02580001,           0x0000000f,
};

data_ov035_021120d8_Layout data_ov035_021120d8 = {{
    0x53504c43, 0x00010008, 0x00001fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0x53504c43, 0x00010008, 0x00001fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0x53504c43, 0x00010008, 0x00001fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0x53504c43, 0x00010008, 0x00001fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0x53504c43, 0x00010008, 0x00001fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0x53504c43, 0x00010008, 0x00001fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0x53504c43, 0x00020008, 0x00000fc5, 0x000000ff, 0x01003fc5, 0x000000ff, 0xffffffff, 0xffffffff,
    0x53504c43, 0x00010008, 0x00001fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
}};
