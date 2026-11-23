#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov031_02111500_Layout;
typedef char data_ov031_02111500_size[sizeof(data_ov031_02111500_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov031_02111504_Layout;
typedef char data_ov031_02111504_size[sizeof(data_ov031_02111504_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov031_02111508_Layout;
typedef char data_ov031_02111508_size[sizeof(data_ov031_02111508_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
} data_ov031_0211150c_Layout;
typedef char data_ov031_0211150c_size[sizeof(data_ov031_0211150c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov031_02111510_Layout;
typedef char data_ov031_02111510_size[sizeof(data_ov031_02111510_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov031_02111518_Layout;
typedef char data_ov031_02111518_size[sizeof(data_ov031_02111518_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
} data_ov031_02111520_Layout;
typedef char data_ov031_02111520_size[sizeof(data_ov031_02111520_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov031_0211152c_Layout;
typedef char data_ov031_0211152c_size[sizeof(data_ov031_0211152c_Layout) == 0x10 ? 1 : -1];

extern data_ov031_02111500_Layout data_ov031_02111500;
extern data_ov031_02111504_Layout data_ov031_02111504;
extern data_ov031_02111508_Layout data_ov031_02111508;
extern data_ov031_0211150c_Layout data_ov031_0211150c;
extern data_ov031_02111510_Layout data_ov031_02111510;
extern data_ov031_02111518_Layout data_ov031_02111518;
extern data_ov031_02111520_Layout data_ov031_02111520;
extern data_ov031_0211152c_Layout data_ov031_0211152c;
extern u8 data_ov031_0211154c[];
extern u8 data_ov031_02111578[];

data_ov031_02111508_Layout data_ov031_02111508 = {{
    0x00000002,
}};

data_ov031_02111500_Layout data_ov031_02111500 = {{
    0x00000756,
}};

data_ov031_02111504_Layout data_ov031_02111504 = {{
    0x11111114,
}};

data_ov031_0211150c_Layout data_ov031_0211150c = {
    &data_ov031_0211154c[0],
};

data_ov031_02111518_Layout data_ov031_02111518 = {{
    0x00000001,
    0x75306d60,
}};

data_ov031_02111510_Layout data_ov031_02111510 = {
    0x00000006,
    &data_ov031_02111578[0],
};

data_ov031_02111520_Layout data_ov031_02111520 = {
    &data_ov031_02111508,
    0x00000000,
    0x35480000,
};

data_ov031_0211152c_Layout data_ov031_0211152c = {{
    0x2d67ff02,
    0x31f532e3,
    0xa0000000,
    0x00000000,
}};
