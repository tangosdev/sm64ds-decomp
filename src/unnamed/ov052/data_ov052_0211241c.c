#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov052_0211241c_Layout;
typedef char data_ov052_0211241c_size[sizeof(data_ov052_0211241c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov052_02112428_Layout;
typedef char data_ov052_02112428_size[sizeof(data_ov052_02112428_Layout) == 0xc ? 1 : -1];

extern data_ov052_0211241c_Layout data_ov052_0211241c;
extern data_ov052_02112428_Layout data_ov052_02112428;

data_ov052_0211241c_Layout data_ov052_0211241c = {{
    0x00005555,
    0xff280000,
    0x118f0005,
}};

data_ov052_02112428_Layout data_ov052_02112428 = {{
    0xf4ac11f8,
    0x00008000,
    0xff280000,
}};
