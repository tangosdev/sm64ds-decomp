#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov058_0211173c_Layout;
typedef char data_ov058_0211173c_size[sizeof(data_ov058_0211173c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
} data_ov058_02111740_Layout;
typedef char data_ov058_02111740_size[sizeof(data_ov058_02111740_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov058_02111744_Layout;
typedef char data_ov058_02111744_size[sizeof(data_ov058_02111744_Layout) == 0x10 ? 1 : -1];

extern data_ov058_0211173c_Layout data_ov058_0211173c;
extern data_ov058_02111740_Layout data_ov058_02111740;
extern data_ov058_02111744_Layout data_ov058_02111744;
extern u8 data_ov058_0211171c[];

data_ov058_02111740_Layout data_ov058_02111740 = {
    &data_ov058_0211171c[0],
};

data_ov058_0211173c_Layout data_ov058_0211173c = {{
    0x00000001,
}};

data_ov058_02111744_Layout data_ov058_02111744 = {{
    0x0244004b,
    0xf83002d5,
    0x4e38ffff,
    0x070d0000,
}};
