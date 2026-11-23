#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} data_ov052_021124c4_Layout;
typedef char data_ov052_021124c4_size[sizeof(data_ov052_021124c4_Layout) == 0x10 ? 1 : -1];

extern data_ov052_021124c4_Layout data_ov052_021124c4;

data_ov052_021124c4_Layout data_ov052_021124c4 = {{
    0x0af0004c,
    0x170c11f8,
    0xc000ffff,
    0x074e0000,
}};
