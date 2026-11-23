#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov019_02112b80_Layout;
typedef char data_ov019_02112b80_size[sizeof(data_ov019_02112b80_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov019_02112b84_Layout;
typedef char data_ov019_02112b84_size[sizeof(data_ov019_02112b84_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov019_02112b88_Layout;
typedef char data_ov019_02112b88_size[sizeof(data_ov019_02112b88_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov019_02112b90_Layout;
typedef char data_ov019_02112b90_size[sizeof(data_ov019_02112b90_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov019_02112b98_Layout;
typedef char data_ov019_02112b98_size[sizeof(data_ov019_02112b98_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov019_02112ba0_Layout;
typedef char data_ov019_02112ba0_size[sizeof(data_ov019_02112ba0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov019_02112ba8_Layout;
typedef char data_ov019_02112ba8_size[sizeof(data_ov019_02112ba8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov019_02112bb0_Layout;
typedef char data_ov019_02112bb0_size[sizeof(data_ov019_02112bb0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov019_02112bb8_Layout;
typedef char data_ov019_02112bb8_size[sizeof(data_ov019_02112bb8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov019_02112bc0_Layout;
typedef char data_ov019_02112bc0_size[sizeof(data_ov019_02112bc0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov019_02112bc8_Layout;
typedef char data_ov019_02112bc8_size[sizeof(data_ov019_02112bc8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
} data_ov019_02112bd0_Layout;
typedef char data_ov019_02112bd0_size[sizeof(data_ov019_02112bd0_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov019_02112bdc_Layout;
typedef char data_ov019_02112bdc_size[sizeof(data_ov019_02112bdc_Layout) == 0xc ? 1 : -1];

extern data_ov019_02112b80_Layout data_ov019_02112b80;
extern data_ov019_02112b84_Layout data_ov019_02112b84;
extern data_ov019_02112b88_Layout data_ov019_02112b88;
extern data_ov019_02112b90_Layout data_ov019_02112b90;
extern data_ov019_02112b98_Layout data_ov019_02112b98;
extern data_ov019_02112ba0_Layout data_ov019_02112ba0;
extern data_ov019_02112ba8_Layout data_ov019_02112ba8;
extern data_ov019_02112bb0_Layout data_ov019_02112bb0;
extern data_ov019_02112bb8_Layout data_ov019_02112bb8;
extern data_ov019_02112bc0_Layout data_ov019_02112bc0;
extern data_ov019_02112bc8_Layout data_ov019_02112bc8;
extern data_ov019_02112bd0_Layout data_ov019_02112bd0;
extern data_ov019_02112bdc_Layout data_ov019_02112bdc;
extern u8 data_ov019_02112db4[];
extern u8 data_ov019_02112c84[];

data_ov019_02112b80_Layout data_ov019_02112b80 = {{
    0x000007c3,
}};

data_ov019_02112b84_Layout data_ov019_02112b84 = {{
    0x31010114,
}};

data_ov019_02112bc0_Layout data_ov019_02112bc0 = {{
    0xe412463f,
    0xe66aea20,
}};

data_ov019_02112bb8_Layout data_ov019_02112bb8 = {{
    0xe412463f,
    0xe66aea20,
}};

data_ov019_02112ba8_Layout data_ov019_02112ba8 = {
    0x00000008,
    &data_ov019_02112c84[0],
};

data_ov019_02112bb0_Layout data_ov019_02112bb0 = {{
    0xe412463f,
    0xe66aea20,
}};

data_ov019_02112ba0_Layout data_ov019_02112ba0 = {
    0x0000000e,
    &data_ov019_02112db4[0],
};

data_ov019_02112b98_Layout data_ov019_02112b98 = {{
    0xe412463f,
    0xe66aea20,
}};

data_ov019_02112b90_Layout data_ov019_02112b90 = {{
    0xe412463f,
    0xe66aea20,
}};

data_ov019_02112bc8_Layout data_ov019_02112bc8 = {{
    0xe412463f,
    0xe66aea20,
}};

data_ov019_02112b88_Layout data_ov019_02112b88 = {{
    0xff560000,
    0x00007fff,
}};

data_ov019_02112bd0_Layout data_ov019_02112bd0 = {
    &data_ov019_02112ba0,
    0x00000000,
    0x1f400000,
};

data_ov019_02112bdc_Layout data_ov019_02112bdc = {{
    0xe9bce2e7,
    0x4000e5a2,
    0x00080000,
}};
