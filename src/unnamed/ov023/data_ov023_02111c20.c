#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov023_02111c20_Layout;
typedef char data_ov023_02111c20_size[sizeof(data_ov023_02111c20_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov023_02111c38_Layout;
typedef char data_ov023_02111c38_size[sizeof(data_ov023_02111c38_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[17];
} data_ov023_02111c54_Layout;
typedef char data_ov023_02111c54_size[sizeof(data_ov023_02111c54_Layout) == 0x44 ? 1 : -1];

extern data_ov023_02111c20_Layout data_ov023_02111c20;
extern data_ov023_02111c38_Layout data_ov023_02111c38;
extern data_ov023_02111c54_Layout data_ov023_02111c54;

data_ov023_02111c20_Layout data_ov023_02111c20 = {{
    0x55550000,
    0x00080000,
    0xfd120000,
    0xfd120064,
    0x55550000,
    0x00080000,
}};

data_ov023_02111c38_Layout data_ov023_02111c38 = {{
    0x53504c43,
    0x00080008,
    0x00030000,
    0x000000ff,
    0x000b0000,
    0x000000ff,
    0x00033000,
}};

data_ov023_02111c54_Layout data_ov023_02111c54 = {{
    0x000000ff,
    0x00032000,
    0x000000ff,
    0x00032004,
    0x000000ff,
    0x00230000,
    0x000000ff,
    0x01032000,
    0x000000ff,
    0x01030000,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
}};
