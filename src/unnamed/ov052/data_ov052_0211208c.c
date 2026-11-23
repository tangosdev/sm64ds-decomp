#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov052_0211208c_Layout;
typedef char data_ov052_0211208c_size[sizeof(data_ov052_0211208c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov052_021120a4_Layout;
typedef char data_ov052_021120a4_size[sizeof(data_ov052_021120a4_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov052_021120bc_Layout;
typedef char data_ov052_021120bc_size[sizeof(data_ov052_021120bc_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov052_021120d8_Layout;
typedef char data_ov052_021120d8_size[sizeof(data_ov052_021120d8_Layout) == 0x20 ? 1 : -1];

extern data_ov052_0211208c_Layout data_ov052_0211208c;
extern data_ov052_021120a4_Layout data_ov052_021120a4;
extern data_ov052_021120bc_Layout data_ov052_021120bc;
extern data_ov052_021120d8_Layout data_ov052_021120d8;

data_ov052_0211208c_Layout data_ov052_0211208c = {{
    0x00000000,
    0xffff0000,
    0xee6c0084,
    0x07d00e74,
    0x00000000,
    0xffff0000,
}};

data_ov052_021120a4_Layout data_ov052_021120a4 = {{
    0xfaec0084,
    0x0fa010cc,
    0x00000000,
    0xffff0000,
    0xfd440084,
    0x125c10cc,
}};

data_ov052_021120bc_Layout data_ov052_021120bc = {{
    0x00000000,
    0xffff0000,
    0xfaec0084,
    0x151810cc,
    0x00000000,
    0xffff0000,
    0x073a0084,
}};

data_ov052_021120d8_Layout data_ov052_021120d8 = {{
    0xff380e74,
    0x20000000,
    0xffff0000,
    0x0a8c0084,
    0xff380e74,
    0x20000000,
    0xffff0000,
    0x0dde0084,
}};
