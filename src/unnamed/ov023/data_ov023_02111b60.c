#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov023_02111b60_Layout;
typedef char data_ov023_02111b60_size[sizeof(data_ov023_02111b60_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
} data_ov023_02111b64_Layout;
typedef char data_ov023_02111b64_size[sizeof(data_ov023_02111b64_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov023_02111b68_Layout;
typedef char data_ov023_02111b68_size[sizeof(data_ov023_02111b68_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
} data_ov023_02111b6c_Layout;
typedef char data_ov023_02111b6c_size[sizeof(data_ov023_02111b6c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov023_02111b70_Layout;
typedef char data_ov023_02111b70_size[sizeof(data_ov023_02111b70_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    u32 f00c;
    void *f010;
} data_ov023_02111b74_Layout;
typedef char data_ov023_02111b74_size[sizeof(data_ov023_02111b74_Layout) == 0x14 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    u32 f00c;
    void *f010;
    u32 f014;
    u32 f018;
} data_ov023_02111b88_Layout;
typedef char data_ov023_02111b88_size[sizeof(data_ov023_02111b88_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov023_02111ba4_Layout;
typedef char data_ov023_02111ba4_size[sizeof(data_ov023_02111ba4_Layout) == 0x1c ? 1 : -1];

extern data_ov023_02111b60_Layout data_ov023_02111b60;
extern data_ov023_02111b64_Layout data_ov023_02111b64;
extern data_ov023_02111b68_Layout data_ov023_02111b68;
extern data_ov023_02111b6c_Layout data_ov023_02111b6c;
extern data_ov023_02111b70_Layout data_ov023_02111b70;
extern data_ov023_02111b74_Layout data_ov023_02111b74;
extern data_ov023_02111b88_Layout data_ov023_02111b88;
extern data_ov023_02111ba4_Layout data_ov023_02111ba4;
extern u8 data_ov023_02111e10[];
extern u8 data_ov023_02111d30[];
extern u8 data_ov023_02111764[];
extern u8 data_ov023_02111760[];
extern u8 data_ov023_02111784[];
extern u8 data_ov023_02111768[];
extern u8 data_ov023_02111c38[];
extern u8 data_ov023_02111b38[];
extern u8 data_ov023_02111b48[];

data_ov023_02111b60_Layout data_ov023_02111b60 = {{
    0x00001900,
}};

data_ov023_02111b64_Layout data_ov023_02111b64 = {
    &data_ov023_02111e10[0],
};

data_ov023_02111b70_Layout data_ov023_02111b70 = {{
    0x000000c8,
}};

data_ov023_02111b6c_Layout data_ov023_02111b6c = {
    &data_ov023_02111d30[0],
};

data_ov023_02111b68_Layout data_ov023_02111b68 = {{
    0x00001c05,
}};

data_ov023_02111b74_Layout data_ov023_02111b74 = {
    &data_ov023_02111764[0], &data_ov023_02111760[0], &data_ov023_02111784[0], 0x00000001,
    &data_ov023_02111768[0],
};

data_ov023_02111b88_Layout data_ov023_02111b88 = {
    &data_ov023_02111c38[0],
    &data_ov023_02111b38[0],
    0x07450748,
    0x074a0749,
    &data_ov023_02111b48[0],
    0x02580001,
    0x0000000f,
};

data_ov023_02111ba4_Layout data_ov023_02111ba4 = {{
    0x0000ff01,
    0x00001388,
    0x00000000,
    0xff020000,
    0x01f4fbb4,
    0x00000000,
    0x00008000,
}};
