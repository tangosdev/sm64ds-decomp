#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov052_02112434_Layout;
typedef char data_ov052_02112434_size[sizeof(data_ov052_02112434_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[5];
} data_ov052_02112438_Layout;
typedef char data_ov052_02112438_size[sizeof(data_ov052_02112438_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov052_0211244c_Layout;
typedef char data_ov052_0211244c_size[sizeof(data_ov052_0211244c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov052_02112464_Layout;
typedef char data_ov052_02112464_size[sizeof(data_ov052_02112464_Layout) == 0x30 ? 1 : -1];

extern data_ov052_02112434_Layout data_ov052_02112434;
extern data_ov052_02112438_Layout data_ov052_02112438;
extern data_ov052_0211244c_Layout data_ov052_0211244c;
extern data_ov052_02112464_Layout data_ov052_02112464;

data_ov052_02112434_Layout data_ov052_02112434 = {{
    0xee6c0005,
}};

data_ov052_02112438_Layout data_ov052_02112438 = {{
    0x00b91004,
    0x00000000,
    0xff190000,
    0xee6c0005,
    0xfc7c1004,
}};

data_ov052_0211244c_Layout data_ov052_0211244c = {{
    0x00004000,
    0xff190000,
    0xee6c0005,
    0xf8301004,
    0x00008000,
    0xff190000,
}};

data_ov052_02112464_Layout data_ov052_02112464 = {{
    0x03200005,
    0xe6ce122a,
    0x00000000,
    0xff320000,
    0xfb82000f,
    0xf4de0fa0,
    0x00000000,
    0xffff0000,
    0x044c00fe,
    0xfa240fa0,
    0x00000000,
    0x00040000,
}};
