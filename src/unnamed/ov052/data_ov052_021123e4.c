#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} data_ov052_021123e4_Layout;
typedef char data_ov052_021123e4_size[sizeof(data_ov052_021123e4_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov052_021123f4_Layout;
typedef char data_ov052_021123f4_size[sizeof(data_ov052_021123f4_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov052_02112404_Layout;
typedef char data_ov052_02112404_size[sizeof(data_ov052_02112404_Layout) == 0x18 ? 1 : -1];

extern data_ov052_021123e4_Layout data_ov052_021123e4;
extern data_ov052_021123f4_Layout data_ov052_021123f4;
extern data_ov052_02112404_Layout data_ov052_02112404;

data_ov052_021123f4_Layout data_ov052_021123f4 = {{
    0x06bb0005,
    0xf4ac11f8,
    0x00000000,
    0xff280000,
}};

data_ov052_021123e4_Layout data_ov052_021123e4 = {{
    0xe9620006,
    0xf3800fa0,
    0x40000000,
    0x00070000,
}};

data_ov052_02112404_Layout data_ov052_02112404 = {{
    0x0a570005,
    0xf4ac11f8,
    0x00002aaa,
    0xff280000,
    0x0df30005,
    0xf4ac11f8,
}};
