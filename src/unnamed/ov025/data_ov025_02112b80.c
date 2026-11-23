#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov025_02112b80_Layout;
typedef char data_ov025_02112b80_size[sizeof(data_ov025_02112b80_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov025_02112b84_Layout;
typedef char data_ov025_02112b84_size[sizeof(data_ov025_02112b84_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov025_02112b8c_Layout;
typedef char data_ov025_02112b8c_size[sizeof(data_ov025_02112b8c_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov025_02112b9c_Layout;
typedef char data_ov025_02112b9c_size[sizeof(data_ov025_02112b9c_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
} data_ov025_02112bac_Layout;
typedef char data_ov025_02112bac_size[sizeof(data_ov025_02112bac_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    void *f00c;
    u32 f010;
    u32 f014;
} data_ov025_02112bbc_Layout;
typedef char data_ov025_02112bbc_size[sizeof(data_ov025_02112bbc_Layout) == 0x18 ? 1 : -1];

extern data_ov025_02112b80_Layout data_ov025_02112b80;
extern data_ov025_02112b84_Layout data_ov025_02112b84;
extern data_ov025_02112b8c_Layout data_ov025_02112b8c;
extern data_ov025_02112b9c_Layout data_ov025_02112b9c;
extern data_ov025_02112bac_Layout data_ov025_02112bac;
extern data_ov025_02112bbc_Layout data_ov025_02112bbc;
extern u8 data_ov025_02112c68[];
extern u8 data_ov025_02112b4c[];
extern u8 data_ov025_02112b54[];
extern u8 data_ov025_02112bd4[];
extern u8 data_ov025_02112b44[];

data_ov025_02112b80_Layout data_ov025_02112b80 = {{
    0x015e0000,
}};

data_ov025_02112b84_Layout data_ov025_02112b84 = {{
    0x0000fc4a,
    0x00000122,
}};

data_ov025_02112b9c_Layout data_ov025_02112b9c = {{
    0x01f3e63d,
    0xfda808a9,
    0x01f3e63d,
    0xfda8ff9c,
}};

data_ov025_02112b8c_Layout data_ov025_02112b8c = {{
    0x5f74616d,
    0x73757972,
    0x61745f61,
    0x0000696b,
}};

data_ov025_02112bac_Layout data_ov025_02112bac = {
    0x00000200,
    &data_ov025_02112c68[0],
    0x00000105,
    &data_ov025_02112b4c[0],
};

data_ov025_02112bbc_Layout data_ov025_02112bbc = {
    &data_ov025_02112b54[0],
    &data_ov025_02112bd4[0],
    0x17700000,
    &data_ov025_02112b44[0],
    0x00000000,
    0x17700003,
};
