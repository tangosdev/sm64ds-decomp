#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[14];
} data_ov052_02112320_Layout;
typedef char data_ov052_02112320_size[sizeof(data_ov052_02112320_Layout) == 0x38 ? 1 : -1];

extern data_ov052_02112320_Layout data_ov052_02112320;

data_ov052_02112320_Layout data_ov052_02112320 = {{
    0x07510000,
    0x0992003b,
    0xea6b1c20,
    0xc0000000,
    0xffff0000,
    0xf3800031,
    0xfda810fe,
    0x00000000,
    0x00f30000,
    0xf18c0031,
    0xe9bc186a,
    0x00000000,
    0x00f80000,
    0xf18c0031,
}};
