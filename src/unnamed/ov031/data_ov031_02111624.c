#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[7];
} data_ov031_02111624_Layout;
typedef char data_ov031_02111624_size[sizeof(data_ov031_02111624_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov031_02111640_Layout;
typedef char data_ov031_02111640_size[sizeof(data_ov031_02111640_Layout) == 0x20 ? 1 : -1];

extern data_ov031_02111624_Layout data_ov031_02111624;
extern data_ov031_02111640_Layout data_ov031_02111640;

data_ov031_02111624_Layout data_ov031_02111624 = {{
    0x000000ff,
    0x00040fc0,
    0x000000ff,
    0x00043fc4,
    0x000000ff,
    0x01008fc4,
    0x000000ff,
}};

data_ov031_02111640_Layout data_ov031_02111640 = {{
    0x01008fc0,
    0x000000ff,
    0x0000dfc0,
    0x000000ff,
    0x0000dfc5,
    0x000000ff,
    0x0000dfc4,
    0x000000ff,
}};
