#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[5];
} data_ov018_02112f68_Layout;
typedef char data_ov018_02112f68_size[sizeof(data_ov018_02112f68_Layout) == 0x14 ? 1 : -1];

extern data_ov018_02112f68_Layout data_ov018_02112f68;

data_ov018_02112f68_Layout data_ov018_02112f68 = {{
    0xeffc00a2,
    0x06fefb3c,
    0xc0000000,
    0xffff0000,
    0x1460000f,
}};
