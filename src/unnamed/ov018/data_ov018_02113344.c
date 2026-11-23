#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[19];
} data_ov018_02113344_Layout;
typedef char data_ov018_02113344_size[sizeof(data_ov018_02113344_Layout) == 0x4c ? 1 : -1];

extern data_ov018_02113344_Layout data_ov018_02113344;

data_ov018_02113344_Layout data_ov018_02113344 = {{
    0x000000ff, 0x00740004, 0x000000ff, 0x00740005, 0x000000ff, 0x00043007, 0x000000ff,
    0x00043004, 0x000000ff, 0x00043006, 0x000000ff, 0x00044006, 0x000000ff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
}};
