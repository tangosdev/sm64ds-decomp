#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov019_02112c44_Layout;
typedef char data_ov019_02112c44_size[sizeof(data_ov019_02112c44_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov019_02112c48_Layout;
typedef char data_ov019_02112c48_size[sizeof(data_ov019_02112c48_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov019_02112c4c_Layout;
typedef char data_ov019_02112c4c_size[sizeof(data_ov019_02112c4c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov019_02112c58_Layout;
typedef char data_ov019_02112c58_size[sizeof(data_ov019_02112c58_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    u32 f00c;
    void *f010;
    u32 f014;
    u32 f018;
} data_ov019_02112c68_Layout;
typedef char data_ov019_02112c68_size[sizeof(data_ov019_02112c68_Layout) == 0x1c ? 1 : -1];

extern data_ov019_02112c44_Layout data_ov019_02112c44;
extern data_ov019_02112c48_Layout data_ov019_02112c48;
extern data_ov019_02112c4c_Layout data_ov019_02112c4c;
extern data_ov019_02112c58_Layout data_ov019_02112c58;
extern data_ov019_02112c68_Layout data_ov019_02112c68;
extern u8 data_ov019_02112d54[];
extern u8 data_ov019_02112ba8[];
extern u8 data_ov019_02112bd0[];

data_ov019_02112c44_Layout data_ov019_02112c44 = {{
    0x03000000,
}};

data_ov019_02112c48_Layout data_ov019_02112c48 = {{
    0xeb4c00a0,
}};

data_ov019_02112c4c_Layout data_ov019_02112c4c = {{
    0xe8901964,
    0xc0000000,
    0x03000000,
}};

data_ov019_02112c58_Layout data_ov019_02112c58 = {{
    0xeb4c00a0,
    0xe8901964,
    0xc0000000,
    0x03000000,
}};

data_ov019_02112c68_Layout data_ov019_02112c68 = {
    &data_ov019_02112d54[0],
    &data_ov019_02112ba8[0],
    0x07c207c4,
    0x07c607c5,
    &data_ov019_02112bd0[0],
    0x06400001,
    0x0000000f,
};
