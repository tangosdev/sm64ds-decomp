#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov019_02112d48_Layout;
typedef char data_ov019_02112d48_size[sizeof(data_ov019_02112d48_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[17];
} data_ov019_02112d54_Layout;
typedef char data_ov019_02112d54_size[sizeof(data_ov019_02112d54_Layout) == 0x44 ? 1 : -1];

extern data_ov019_02112d48_Layout data_ov019_02112d48;
extern data_ov019_02112d54_Layout data_ov019_02112d54;

data_ov019_02112d48_Layout data_ov019_02112d48 = {{
    0xfda106e4,
    0x00000000,
    0xffff0000,
}};

data_ov019_02112d54_Layout data_ov019_02112d54 = {{
    0x53504c43,
    0x00080008,
    0x0005cfc7,
    0x000000ff,
    0x00040fc0,
    0x000000ff,
    0x01000fc0,
    0x000000ff,
    0x00200fc0,
    0x000000ff,
    0x01043fc4,
    0x000000ff,
    0x00043fc4,
    0x000000ff,
    0x00043fc5,
    0x000000ff,
    0x0000dfc7,
}};
