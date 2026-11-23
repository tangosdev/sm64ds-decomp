#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov019_02112be8_Layout;
typedef char data_ov019_02112be8_size[sizeof(data_ov019_02112be8_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov019_02112bec_Layout;
typedef char data_ov019_02112bec_size[sizeof(data_ov019_02112bec_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov019_02112bf8_Layout;
typedef char data_ov019_02112bf8_size[sizeof(data_ov019_02112bf8_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov019_02112c08_Layout;
typedef char data_ov019_02112c08_size[sizeof(data_ov019_02112c08_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov019_02112c18_Layout;
typedef char data_ov019_02112c18_size[sizeof(data_ov019_02112c18_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov019_02112c28_Layout;
typedef char data_ov019_02112c28_size[sizeof(data_ov019_02112c28_Layout) == 0x10 ? 1 : -1];

extern data_ov019_02112be8_Layout data_ov019_02112be8;
extern data_ov019_02112bec_Layout data_ov019_02112bec;
extern data_ov019_02112bf8_Layout data_ov019_02112bf8;
extern data_ov019_02112c08_Layout data_ov019_02112c08;
extern data_ov019_02112c18_Layout data_ov019_02112c18;
extern data_ov019_02112c28_Layout data_ov019_02112c28;

data_ov019_02112be8_Layout data_ov019_02112be8 = {{
    0xeb4cff02,
}};

data_ov019_02112bec_Layout data_ov019_02112bec = {{
    0xeab619c8,
    0xa0000000,
    0x00000000,
}};

data_ov019_02112c08_Layout data_ov019_02112c08 = {{
    0xeb4c00a0,
    0xe8901964,
    0xc0000000,
    0x03000000,
}};

data_ov019_02112bf8_Layout data_ov019_02112bf8 = {{
    0xe9bce2e6,
    0x0000e55c,
    0x040a4000,
    0x000021ff,
}};

data_ov019_02112c18_Layout data_ov019_02112c18 = {{
    0xeb4c00a0,
    0xe8901964,
    0xc0000000,
    0x03000000,
}};

data_ov019_02112c28_Layout data_ov019_02112c28 = {{
    0xeb4c00a0,
    0xe8901964,
    0xc0000000,
    0x03000000,
}};
