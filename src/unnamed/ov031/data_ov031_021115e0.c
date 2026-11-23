#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov031_021115e0_Layout;
typedef char data_ov031_021115e0_size[sizeof(data_ov031_021115e0_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov031_021115e4_Layout;
typedef char data_ov031_021115e4_size[sizeof(data_ov031_021115e4_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov031_021115e8_Layout;
typedef char data_ov031_021115e8_size[sizeof(data_ov031_021115e8_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov031_021115f4_Layout;
typedef char data_ov031_021115f4_size[sizeof(data_ov031_021115f4_Layout) == 0x30 ? 1 : -1];

extern data_ov031_021115e0_Layout data_ov031_021115e0;
extern data_ov031_021115e4_Layout data_ov031_021115e4;
extern data_ov031_021115e8_Layout data_ov031_021115e8;
extern data_ov031_021115f4_Layout data_ov031_021115f4;

data_ov031_021115e0_Layout data_ov031_021115e0 = {{
    0x9c720000,
}};

#pragma explicit_zero_data on
data_ov031_021115e4_Layout data_ov031_021115e4 = {0};
#pragma explicit_zero_data off

data_ov031_021115e8_Layout data_ov031_021115e8 = {{
    0x53504c43,
    0x000e0008,
    0x00003fc4,
}};

data_ov031_021115f4_Layout data_ov031_021115f4 = {{
    0x000000ff,
    0x0005bfc4,
    0x000000ff,
    0x01000fc0,
    0x000000ff,
    0x04000fc0,
    0x000000ff,
    0x00200fc0,
    0x000000ff,
    0x01040fc4,
    0x000000ff,
    0x00043fc0,
}};
