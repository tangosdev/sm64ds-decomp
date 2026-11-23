#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov023_02111b20_Layout;
typedef char data_ov023_02111b20_size[sizeof(data_ov023_02111b20_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov023_02111b24_Layout;
typedef char data_ov023_02111b24_size[sizeof(data_ov023_02111b24_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov023_02111b28_Layout;
typedef char data_ov023_02111b28_size[sizeof(data_ov023_02111b28_Layout) == 0x8 ? 1 : -1];

extern data_ov023_02111b20_Layout data_ov023_02111b20;
extern data_ov023_02111b24_Layout data_ov023_02111b24;
extern data_ov023_02111b28_Layout data_ov023_02111b28;
extern u8 data_ov023_02111b60[];

data_ov023_02111b24_Layout data_ov023_02111b24 = {{
    0x03030304,
}};

data_ov023_02111b20_Layout data_ov023_02111b20 = {{
    0x07470746,
}};

data_ov023_02111b28_Layout data_ov023_02111b28 = {
    0x00000002,
    &data_ov023_02111b60[0],
};
