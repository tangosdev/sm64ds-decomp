#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov039_021115e0_Layout;
typedef char data_ov039_021115e0_size[sizeof(data_ov039_021115e0_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov039_021115e4_Layout;
typedef char data_ov039_021115e4_size[sizeof(data_ov039_021115e4_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov039_021115f4_Layout;
typedef char data_ov039_021115f4_size[sizeof(data_ov039_021115f4_Layout) == 0x30 ? 1 : -1];

extern data_ov039_021115e0_Layout data_ov039_021115e0;
extern data_ov039_021115e4_Layout data_ov039_021115e4;
extern data_ov039_021115f4_Layout data_ov039_021115f4;

#pragma explicit_zero_data on
data_ov039_021115e0_Layout data_ov039_021115e0 = {0};
#pragma explicit_zero_data off

data_ov039_021115e4_Layout data_ov039_021115e4 = {{
    0xffff0000,
    0xf7680106,
    0xf63cfea2,
    0x00000000,
}};

data_ov039_021115f4_Layout data_ov039_021115f4 = {{
    0xffff0000,
    0x03e80106,
    0xf448f8c6,
    0x00000000,
    0xffff0000,
    0x09c40106,
    0xf254f2ea,
    0x00000000,
    0xffff0000,
    0xfe0c0106,
    0xf060fcae,
    0x00000000,
}};
