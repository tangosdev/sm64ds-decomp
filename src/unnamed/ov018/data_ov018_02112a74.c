#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov018_02112a74_Layout;
typedef char data_ov018_02112a74_size[sizeof(data_ov018_02112a74_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov018_02112a78_Layout;
typedef char data_ov018_02112a78_size[sizeof(data_ov018_02112a78_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
    u32 f018;
} data_ov018_02112a7c_Layout;
typedef char data_ov018_02112a7c_size[sizeof(data_ov018_02112a7c_Layout) == 0x1c ? 1 : -1];

extern const data_ov018_02112a74_Layout data_ov018_02112a74;
extern const data_ov018_02112a78_Layout data_ov018_02112a78;
extern const data_ov018_02112a7c_Layout data_ov018_02112a7c;
extern u8 data_ov018_02112ea8[];

const data_ov018_02112a78_Layout data_ov018_02112a78 = {{
    0x00001000,
}};

const data_ov018_02112a74_Layout data_ov018_02112a74 = {{
    0x00000000,
}};

const data_ov018_02112a7c_Layout data_ov018_02112a7c = {
    0x0000ffff, &data_ov018_02112ea8[0], 0x00000001, 0x00000001, 0x00000001, 0x0000005b, 0x0000005b,
};
