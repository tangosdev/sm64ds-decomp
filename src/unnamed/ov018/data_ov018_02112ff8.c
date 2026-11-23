#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov018_02112ff8_Layout;
typedef char data_ov018_02112ff8_size[sizeof(data_ov018_02112ff8_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[11];
} data_ov018_02112ffc_Layout;
typedef char data_ov018_02112ffc_size[sizeof(data_ov018_02112ffc_Layout) == 0x2c ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov018_02113028_Layout;
typedef char data_ov018_02113028_size[sizeof(data_ov018_02113028_Layout) == 0x30 ? 1 : -1];

typedef struct {
    u32 w[14];
} data_ov018_02113058_Layout;
typedef char data_ov018_02113058_size[sizeof(data_ov018_02113058_Layout) == 0x38 ? 1 : -1];

typedef struct {
    u32 w[15];
} data_ov018_02113090_Layout;
typedef char data_ov018_02113090_size[sizeof(data_ov018_02113090_Layout) == 0x3c ? 1 : -1];

extern data_ov018_02112ff8_Layout data_ov018_02112ff8;
extern data_ov018_02112ffc_Layout data_ov018_02112ffc;
extern data_ov018_02113028_Layout data_ov018_02113028;
extern data_ov018_02113058_Layout data_ov018_02113058;
extern data_ov018_02113090_Layout data_ov018_02113090;

data_ov018_02112ff8_Layout data_ov018_02112ff8 = {{
    0xeffc00a2,
}};

data_ov018_02112ffc_Layout data_ov018_02112ffc = {{
    0x06fefb3c,
    0xc0000000,
    0xffff0000,
    0x1460000f,
    0x01ecee6c,
    0x40000000,
    0xf1f00000,
    0xf31c0014,
    0xf894fb32,
    0xa0000003,
    0xffff0000,
}};

data_ov018_02113028_Layout data_ov018_02113028 = {{
    0x1460000f,
    0x01ecee6c,
    0x40000000,
    0xf1f00000,
    0x09c400a4,
    0xfbe60a96,
    0x00000000,
    0x00020000,
    0xeffc00a3,
    0x06fefb3c,
    0xc0000000,
    0x00050000,
}};

data_ov018_02113058_Layout data_ov018_02113058 = {{
    0x0000ff00,
    0x00001770,
    0x00000000,
    0xff020000,
    0x0bb8f542,
    0x0000f31c,
    0x0000271c,
    0xffceff07,
    0xff6a05d2,
    0x00000000,
    0xff020000,
    0xefbd0edc,
    0x0000135c,
    0x0000aaab,
}};

data_ov018_02113090_Layout data_ov018_02113090 = {{
    0xfda90014,
    0xf67a09c4,
    0xc00000ff,
    0x0b260000,
    0x132c00fe,
    0x0492ee6c,
    0x80000000,
    0x00040000,
    0xeffc00a2,
    0x06fefb3c,
    0xc0000000,
    0xffff0000,
    0x1460000f,
    0x01ecee6c,
    0x40000000,
}};
