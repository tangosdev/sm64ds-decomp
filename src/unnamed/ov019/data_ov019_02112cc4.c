#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov019_02112cc4_Layout;
typedef char data_ov019_02112cc4_size[sizeof(data_ov019_02112cc4_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov019_02112cc8_Layout;
typedef char data_ov019_02112cc8_size[sizeof(data_ov019_02112cc8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[13];
} data_ov019_02112cd0_Layout;
typedef char data_ov019_02112cd0_size[sizeof(data_ov019_02112cd0_Layout) == 0x34 ? 1 : -1];

typedef struct {
    u32 w[17];
} data_ov019_02112d04_Layout;
typedef char data_ov019_02112d04_size[sizeof(data_ov019_02112d04_Layout) == 0x44 ? 1 : -1];

extern data_ov019_02112cc4_Layout data_ov019_02112cc4;
extern data_ov019_02112cc8_Layout data_ov019_02112cc8;
extern data_ov019_02112cd0_Layout data_ov019_02112cd0;
extern data_ov019_02112d04_Layout data_ov019_02112d04;

data_ov019_02112cc4_Layout data_ov019_02112cc4 = {{
    0xe9bc0000,
}};

data_ov019_02112cc8_Layout data_ov019_02112cc8 = {{
    0xe8901e3d,
    0x40000000,
}};

data_ov019_02112cd0_Layout data_ov019_02112cd0 = {{
    0x02000000,
    0xe9bc0000,
    0xe82c1964,
    0x40000000,
    0x00000000,
    0xe9bc0000,
    0xe8f41964,
    0x40000000,
    0x00000000,
    0xe9bc0000,
    0xe9bc1964,
    0x40000000,
    0x00000000,
}};

data_ov019_02112d04_Layout data_ov019_02112d04 = {{
    0xeb4c004c,
    0xe7001964,
    0x0000ffff,
    0x07410000,
    0xed400031,
    0x0d4800ed,
    0x00000000,
    0x00f20000,
    0xeab6001f,
    0xe69cee9e,
    0x00000000,
    0xff020000,
    0xe4120031,
    0x1356fab3,
    0x60000000,
    0x00f20000,
    0xed6d0135,
}};
