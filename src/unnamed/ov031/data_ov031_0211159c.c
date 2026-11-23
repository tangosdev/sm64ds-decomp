#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
    void *f008;
} data_ov031_0211159c_Layout;
typedef char data_ov031_0211159c_size[sizeof(data_ov031_0211159c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[5];
} data_ov031_021115a8_Layout;
typedef char data_ov031_021115a8_size[sizeof(data_ov031_021115a8_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[9];
} data_ov031_021115bc_Layout;
typedef char data_ov031_021115bc_size[sizeof(data_ov031_021115bc_Layout) == 0x24 ? 1 : -1];

extern data_ov031_0211159c_Layout data_ov031_0211159c;
extern data_ov031_021115a8_Layout data_ov031_021115a8;
extern data_ov031_021115bc_Layout data_ov031_021115bc;
extern u8 data_ov031_02111504[];
extern u8 data_ov031_0211153c[];

data_ov031_0211159c_Layout data_ov031_0211159c = {
    &data_ov031_02111504[0],
    0x0000010a,
    &data_ov031_0211153c[0],
};

data_ov031_021115a8_Layout data_ov031_021115a8 = {{
    0x2c6d0000,
    0x30fb3473,
    0x9c720000,
    0x02000000,
    0x2c090000,
}};

data_ov031_021115bc_Layout data_ov031_021115bc = {{
    0x31c3322d,
    0x9c720000,
    0x00000000,
    0x2d670000,
    0x30c9321b,
    0x9c720000,
    0x00000000,
    0x2d030000,
    0x3191321b,
}};
