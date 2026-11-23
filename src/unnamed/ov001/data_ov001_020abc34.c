#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov001_020abc34_Layout;
typedef char data_ov001_020abc34_size[sizeof(data_ov001_020abc34_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov001_020abc3c_Layout;
typedef char data_ov001_020abc3c_size[sizeof(data_ov001_020abc3c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov001_020abc44_Layout;
typedef char data_ov001_020abc44_size[sizeof(data_ov001_020abc44_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov001_020abc4c_Layout;
typedef char data_ov001_020abc4c_size[sizeof(data_ov001_020abc4c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov001_020abc64_Layout;
typedef char data_ov001_020abc64_size[sizeof(data_ov001_020abc64_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov001_020abc7c_Layout;
typedef char data_ov001_020abc7c_size[sizeof(data_ov001_020abc7c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov001_020abc94_Layout;
typedef char data_ov001_020abc94_size[sizeof(data_ov001_020abc94_Layout) == 0x20 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
    void *f010;
    void *f014;
    void *f018;
    void *f01c;
    void *f020;
} data_ov001_020abcb4_Layout;
typedef char data_ov001_020abcb4_size[sizeof(data_ov001_020abcb4_Layout) == 0x24 ? 1 : -1];

typedef struct {
    u32 w[16];
} data_ov001_020abcd8_Layout;
typedef char data_ov001_020abcd8_size[sizeof(data_ov001_020abcd8_Layout) == 0x40 ? 1 : -1];

typedef struct {
    u32 w[24];
} data_ov001_020abd18_Layout;
typedef char data_ov001_020abd18_size[sizeof(data_ov001_020abd18_Layout) == 0x60 ? 1 : -1];

extern data_ov001_020abc34_Layout data_ov001_020abc34;
extern data_ov001_020abc3c_Layout data_ov001_020abc3c;
extern data_ov001_020abc44_Layout data_ov001_020abc44;
extern data_ov001_020abc4c_Layout data_ov001_020abc4c;
extern data_ov001_020abc64_Layout data_ov001_020abc64;
extern data_ov001_020abc7c_Layout data_ov001_020abc7c;
extern data_ov001_020abc94_Layout data_ov001_020abc94;
extern data_ov001_020abcb4_Layout data_ov001_020abcb4;
extern data_ov001_020abcd8_Layout data_ov001_020abcd8;
extern data_ov001_020abd18_Layout data_ov001_020abd18;

data_ov001_020abc44_Layout data_ov001_020abc44 = {{
    0x41f800f0,
    0xffffa01a,
}};

data_ov001_020abc34_Layout data_ov001_020abc34 = {{
    0x41f800f0,
    0xffffb01c,
}};

data_ov001_020abc3c_Layout data_ov001_020abc3c = {{
    0x41f800f0,
    0xffffc01e,
}};

data_ov001_020abc7c_Layout data_ov001_020abc7c = {{
    0x81e800e8,
    0x00009014,
    0x800880e8,
    0x00009018,
    0x41f80008,
    0xffff9096,
}};

data_ov001_020abc4c_Layout data_ov001_020abc4c = {{
    0x81e800e8,
    0x00009008,
    0x800880e8,
    0x0000900c,
    0x41f80008,
    0xffff908a,
}};

data_ov001_020abc64_Layout data_ov001_020abc64 = {{
    0x81e800e8,
    0x0000900e,
    0x800880e8,
    0x00009012,
    0x41f80008,
    0xffff9090,
}};

data_ov001_020abc94_Layout data_ov001_020abc94 = {{
    0x41f800c0,
    0x0000a01a,
    0x81e800b8,
    0x00009008,
    0x800880b8,
    0x0000900c,
    0x41f800d8,
    0xffff908a,
}};

data_ov001_020abcb4_Layout data_ov001_020abcb4 = {
    &data_ov001_020abc4c, &data_ov001_020abc64, &data_ov001_020abc7c,
    &data_ov001_020abc44, &data_ov001_020abc34, &data_ov001_020abc3c,
    &data_ov001_020abc94, &data_ov001_020abcd8, &data_ov001_020abd18,
};

data_ov001_020abcd8_Layout data_ov001_020abcd8 = {{
    0x41d800c0,
    0x0000b01c,
    0x81c800b8,
    0x0000900e,
    0x81e880b8,
    0x00009012,
    0x41d800d8,
    0x00009090,
    0x401800c0,
    0x0000c01e,
    0x800800b8,
    0x00009014,
    0x802880b8,
    0x00009018,
    0x401800d8,
    0xffff9096,
}};

data_ov001_020abd18_Layout data_ov001_020abd18 = {{
    0x41f800c0, 0x0000a01a, 0x81e800b8, 0x00009008, 0x800880b8, 0x0000900c, 0x41f800d8, 0x0000908a,
    0x41b000d8, 0x0000b01c, 0x81a000d0, 0x0000900e, 0x81c080d0, 0x00009012, 0x41b000f0, 0x00009090,
    0x404000d8, 0x0000c01e, 0x803000d0, 0x00009014, 0x805080d0, 0x00009018, 0x404000f0, 0xffff9096,
}};
