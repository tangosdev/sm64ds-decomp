#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov025_02112b40_Layout;
typedef char data_ov025_02112b40_size[sizeof(data_ov025_02112b40_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov025_02112b44_Layout;
typedef char data_ov025_02112b44_size[sizeof(data_ov025_02112b44_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov025_02112b4c_Layout;
typedef char data_ov025_02112b4c_size[sizeof(data_ov025_02112b4c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov025_02112b54_Layout;
typedef char data_ov025_02112b54_size[sizeof(data_ov025_02112b54_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov025_02112b5c_Layout;
typedef char data_ov025_02112b5c_size[sizeof(data_ov025_02112b5c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov025_02112b64_Layout;
typedef char data_ov025_02112b64_size[sizeof(data_ov025_02112b64_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov025_02112b6c_Layout;
typedef char data_ov025_02112b6c_size[sizeof(data_ov025_02112b6c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov025_02112b74_Layout;
typedef char data_ov025_02112b74_size[sizeof(data_ov025_02112b74_Layout) == 0xc ? 1 : -1];

extern data_ov025_02112b40_Layout data_ov025_02112b40;
extern data_ov025_02112b44_Layout data_ov025_02112b44;
extern data_ov025_02112b4c_Layout data_ov025_02112b4c;
extern data_ov025_02112b54_Layout data_ov025_02112b54;
extern data_ov025_02112b5c_Layout data_ov025_02112b5c;
extern data_ov025_02112b64_Layout data_ov025_02112b64;
extern data_ov025_02112b6c_Layout data_ov025_02112b6c;
extern data_ov025_02112b74_Layout data_ov025_02112b74;
extern u8 data_ov025_02112bac[];
extern u8 data_ov025_02112c48[];
extern u8 data_ov025_02112d98[];

data_ov025_02112b40_Layout data_ov025_02112b40 = {{
    0x00100104,
}};

data_ov025_02112b64_Layout data_ov025_02112b64 = {{
    0x070807d0,
    0x06400640,
}};

data_ov025_02112b5c_Layout data_ov025_02112b5c = {
    0x00000008,
    &data_ov025_02112d98[0],
};

data_ov025_02112b54_Layout data_ov025_02112b54 = {
    0x00000004,
    &data_ov025_02112c48[0],
};

data_ov025_02112b4c_Layout data_ov025_02112b4c = {{
    0x0000483f,
    0xf18cfb82,
}};

data_ov025_02112b44_Layout data_ov025_02112b44 = {
    0x00000002,
    &data_ov025_02112bac[0],
};

data_ov025_02112b6c_Layout data_ov025_02112b6c = {{
    0x07180717,
    0x071a0719,
}};

data_ov025_02112b74_Layout data_ov025_02112b74 = {{
    0x5f74616d,
    0x73757972,
    0x00000061,
}};
