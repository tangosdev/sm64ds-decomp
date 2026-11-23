#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[9];
} data_ov052_02112258_Layout;
typedef char data_ov052_02112258_size[sizeof(data_ov052_02112258_Layout) == 0x24 ? 1 : -1];

extern data_ov052_02112258_Layout data_ov052_02112258;

data_ov052_02112258_Layout data_ov052_02112258 = {{
    0x125c10cc,
    0x00000000,
    0xffff0000,
    0x0834001e,
    0x09c41450,
    0xc0000000,
    0xff060000,
    0xf380003b,
    0xff9c0fa0,
}};
