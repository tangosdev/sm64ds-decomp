#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov025_02112c48_Layout;
typedef char data_ov025_02112c48_size[sizeof(data_ov025_02112c48_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    void *f008;
} data_ov025_02112c4c_Layout;
typedef char data_ov025_02112c4c_size[sizeof(data_ov025_02112c4c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
} data_ov025_02112c58_Layout;
typedef char data_ov025_02112c58_size[sizeof(data_ov025_02112c58_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov025_02112c68_Layout;
typedef char data_ov025_02112c68_size[sizeof(data_ov025_02112c68_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov025_02112c88_Layout;
typedef char data_ov025_02112c88_size[sizeof(data_ov025_02112c88_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[16];
} data_ov025_02112ca8_Layout;
typedef char data_ov025_02112ca8_size[sizeof(data_ov025_02112ca8_Layout) == 0x40 ? 1 : -1];

extern data_ov025_02112c48_Layout data_ov025_02112c48;
extern data_ov025_02112c4c_Layout data_ov025_02112c4c;
extern data_ov025_02112c58_Layout data_ov025_02112c58;
extern data_ov025_02112c68_Layout data_ov025_02112c68;
extern data_ov025_02112c88_Layout data_ov025_02112c88;
extern data_ov025_02112ca8_Layout data_ov025_02112ca8;
extern u8 data_ov025_02113154[];
extern u8 data_ov025_02112fec[];
extern u8 data_ov025_02112b9c[];
extern u8 data_ov025_02112d68[];

data_ov025_02112c48_Layout data_ov025_02112c48 = {{
    0x00002b00,
}};

data_ov025_02112c4c_Layout data_ov025_02112c4c = {
    &data_ov025_02113154[0],
    0x00002d05,
    &data_ov025_02112fec[0],
};

data_ov025_02112c58_Layout data_ov025_02112c58 = {
    0x00000265,
    &data_ov025_02112b9c[0],
    0x000006c5,
    &data_ov025_02112d68[0],
};

data_ov025_02112c68_Layout data_ov025_02112c68 = {{
    0x00000117,
    0xf380fa24,
    0x00000000,
    0x40000000,
    0x02140108,
    0xf14efbce,
    0x00000000,
    0xffff0000,
}};

data_ov025_02112c88_Layout data_ov025_02112c88 = {{
    0x53504c43,
    0x00010008,
    0x01000fc4,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
}};

data_ov025_02112ca8_Layout data_ov025_02112ca8 = {{
    0x53504c43,
    0x00010008,
    0x01000fc4,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0x53504c43,
    0x00010008,
    0x01000fc4,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
}};
