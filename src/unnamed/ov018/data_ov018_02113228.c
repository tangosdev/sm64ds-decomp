#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[18];
} data_ov018_02113228_Layout;
typedef char data_ov018_02113228_size[sizeof(data_ov018_02113228_Layout) == 0x48 ? 1 : -1];

typedef struct {
    u32 w[31];
} data_ov018_02113270_Layout;
typedef char data_ov018_02113270_size[sizeof(data_ov018_02113270_Layout) == 0x7c ? 1 : -1];

extern data_ov018_02113228_Layout data_ov018_02113228;
extern data_ov018_02113270_Layout data_ov018_02113270;

data_ov018_02113228_Layout data_ov018_02113228 = {{
    0x01cbfb82,
    0x0320f510,
    0xf47a0089,
    0xfe420e10,
    0x1068f574,
    0xf380ff87,
    0xfe230e42,
    0x02eef1f0,
    0xf128fbe6,
    0xfb60f448,
    0xefcaf128,
    0xf5d8faff,
    0xfadeed0e,
    0xeffcffce,
    0x06fefb3c,
    0xfa1e026f,
    0xfb6913c1,
    0x11a5f9c9,
}};

data_ov018_02113270_Layout data_ov018_02113270 = {{
    0x53504c43, 0x00200008, 0x01000fc0, 0x000000ff, 0x00000fc0, 0x000000ff, 0x00200fc0, 0x000000ff,
    0x00980fc0, 0x000000ff, 0x00010fe0, 0x000000ff, 0x00712fc4, 0x000000ff, 0x00712004, 0x000000ff,
    0x00050fc7, 0x000000ff, 0x01050fc5, 0x000000ff, 0x01050fc7, 0x000000ff, 0x00050fc4, 0x000000ff,
    0x00053fc6, 0x000000ff, 0x00050fc6, 0x000000ff, 0x00754fc7, 0x000000ff, 0x00750fc1,
}};
