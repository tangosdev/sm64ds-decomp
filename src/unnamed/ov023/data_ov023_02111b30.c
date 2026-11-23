#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov023_02111b30_Layout;
typedef char data_ov023_02111b30_size[sizeof(data_ov023_02111b30_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov023_02111b34_Layout;
typedef char data_ov023_02111b34_size[sizeof(data_ov023_02111b34_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov023_02111b38_Layout;
typedef char data_ov023_02111b38_size[sizeof(data_ov023_02111b38_Layout) == 0x8 ? 1 : -1];

extern data_ov023_02111b30_Layout data_ov023_02111b30;
extern data_ov023_02111b34_Layout data_ov023_02111b34;
extern data_ov023_02111b38_Layout data_ov023_02111b38;
extern u8 data_ov023_02111bc0[];

data_ov023_02111b34_Layout data_ov023_02111b34 = {{
    0x0031306e,
}};

data_ov023_02111b30_Layout data_ov023_02111b30 = {{
    0x61676f79,
}};

data_ov023_02111b38_Layout data_ov023_02111b38 = {
    0x00000007,
    &data_ov023_02111bc0[0],
};
