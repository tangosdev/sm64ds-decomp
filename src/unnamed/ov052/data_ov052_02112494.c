#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov052_02112494_Layout;
typedef char data_ov052_02112494_size[sizeof(data_ov052_02112494_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov052_021124ac_Layout;
typedef char data_ov052_021124ac_size[sizeof(data_ov052_021124ac_Layout) == 0x18 ? 1 : -1];

extern data_ov052_02112494_Layout data_ov052_02112494;
extern data_ov052_021124ac_Layout data_ov052_021124ac;

data_ov052_02112494_Layout data_ov052_02112494 = {{
    0x064000fe,
    0x0a8c1004,
    0xd5560000,
    0x01040000,
    0xe25000fe,
    0xf47a1004,
}};

data_ov052_021124ac_Layout data_ov052_021124ac = {{
    0x55550000,
    0x02040000,
    0x0064004c,
    0x193211f8,
    0x8000ffff,
    0x074b0000,
}};
