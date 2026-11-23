#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov023_02111b40_Layout;
typedef char data_ov023_02111b40_size[sizeof(data_ov023_02111b40_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov023_02111b44_Layout;
typedef char data_ov023_02111b44_size[sizeof(data_ov023_02111b44_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
} data_ov023_02111b48_Layout;
typedef char data_ov023_02111b48_size[sizeof(data_ov023_02111b48_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov023_02111b54_Layout;
typedef char data_ov023_02111b54_size[sizeof(data_ov023_02111b54_Layout) == 0xc ? 1 : -1];

extern data_ov023_02111b40_Layout data_ov023_02111b40;
extern data_ov023_02111b44_Layout data_ov023_02111b44;
extern data_ov023_02111b48_Layout data_ov023_02111b48;
extern data_ov023_02111b54_Layout data_ov023_02111b54;
extern u8 data_ov023_02111b28[];
extern u8 data_ov023_02111b70[];

data_ov023_02111b40_Layout data_ov023_02111b40 = {{
    0x00008001,
}};

data_ov023_02111b44_Layout data_ov023_02111b44 = {{
    0x4e201d4c,
}};

data_ov023_02111b48_Layout data_ov023_02111b48 = {
    &data_ov023_02111b28[0],
    &data_ov023_02111b70[0],
    0x1c200000,
};

data_ov023_02111b54_Layout data_ov023_02111b54 = {{
    0xff0e0000,
    0x000e7fff,
    0x7fffff0b,
}};
