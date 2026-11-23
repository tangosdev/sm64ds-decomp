#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
    void *f008;
} data_ov023_02111bd4_Layout;
typedef char data_ov023_02111bd4_size[sizeof(data_ov023_02111bd4_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
    u32 f010;
    void *f014;
} data_ov023_02111be0_Layout;
typedef char data_ov023_02111be0_size[sizeof(data_ov023_02111be0_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[10];
} data_ov023_02111bf8_Layout;
typedef char data_ov023_02111bf8_size[sizeof(data_ov023_02111bf8_Layout) == 0x28 ? 1 : -1];

extern data_ov023_02111bd4_Layout data_ov023_02111bd4;
extern data_ov023_02111be0_Layout data_ov023_02111be0;
extern data_ov023_02111bf8_Layout data_ov023_02111bf8;
extern u8 data_ov023_02111ba4[];
extern u8 data_ov023_02111b20[];
extern u8 data_ov023_02111b40[];
extern u8 data_ov023_02111b24[];

data_ov023_02111bd4_Layout data_ov023_02111bd4 = {
    &data_ov023_02111ba4[0],
    0x0000020b,
    &data_ov023_02111b20[0],
};

data_ov023_02111be0_Layout data_ov023_02111be0 = {
    0x00000401, &data_ov023_02111bf8,    0x00000108, &data_ov023_02111b40[0],
    0x0000010e, &data_ov023_02111b24[0],
};

data_ov023_02111bf8_Layout data_ov023_02111bf8 = {{
    0xfc7c0000,
    0xfc180708,
    0x55550000,
    0x02080000,
    0xfb820000,
    0xfc7c0064,
    0x55550000,
    0x00080000,
    0xfc180000,
    0xfd120064,
}};
