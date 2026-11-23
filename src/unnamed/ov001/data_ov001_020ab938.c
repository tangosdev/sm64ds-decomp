#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov001_020ab938_Layout;
typedef char data_ov001_020ab938_size[sizeof(data_ov001_020ab938_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov001_020ab940_Layout;
typedef char data_ov001_020ab940_size[sizeof(data_ov001_020ab940_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
} _ZN3OAM10LIFE_ICONSE_Layout;
typedef char _ZN3OAM10LIFE_ICONSE_size[sizeof(_ZN3OAM10LIFE_ICONSE_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov001_020ab958_Layout;
typedef char data_ov001_020ab958_size[sizeof(data_ov001_020ab958_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov001_020ab968_Layout;
typedef char data_ov001_020ab968_size[sizeof(data_ov001_020ab968_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov001_020ab988_Layout;
typedef char data_ov001_020ab988_size[sizeof(data_ov001_020ab988_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov001_020ab9a8_Layout;
typedef char data_ov001_020ab9a8_size[sizeof(data_ov001_020ab9a8_Layout) == 0x20 ? 1 : -1];

extern data_ov001_020ab938_Layout data_ov001_020ab938;
extern data_ov001_020ab940_Layout data_ov001_020ab940;
extern _ZN3OAM10LIFE_ICONSE_Layout _ZN3OAM10LIFE_ICONSE;
extern data_ov001_020ab958_Layout data_ov001_020ab958;
extern data_ov001_020ab968_Layout data_ov001_020ab968;
extern data_ov001_020ab988_Layout data_ov001_020ab988;
extern data_ov001_020ab9a8_Layout data_ov001_020ab9a8;

data_ov001_020ab938_Layout data_ov001_020ab938 = {{
    0x41f800f8,
    0xffff6000,
}};

data_ov001_020ab940_Layout data_ov001_020ab940 = {{
    0x41f800f8,
    0xffff6002,
}};

data_ov001_020ab958_Layout data_ov001_020ab958 = {{
    0x41f000f8,
    0x0000d072,
    0x000080f8,
    0xffffd074,
}};

_ZN3OAM10LIFE_ICONSE_Layout _ZN3OAM10LIFE_ICONSE = {
    &data_ov001_020ab988,
    &data_ov001_020ab9a8,
    &data_ov001_020ab968,
    &data_ov001_020ab958,
};

data_ov001_020ab968_Layout data_ov001_020ab968 = {{
    0x41f000f4,
    0x0000c06f,
    0x000080f4,
    0x0000c071,
    0x01f04004,
    0x0000c0af,
    0x00000004,
    0xffffc0b1,
}};

data_ov001_020ab988_Layout data_ov001_020ab988 = {{
    0x41f000f4,
    0x0000a069,
    0x000080f4,
    0x0000a06b,
    0x01f04004,
    0x0000a0a9,
    0x00000004,
    0xffffa0ab,
}};

data_ov001_020ab9a8_Layout data_ov001_020ab9a8 = {{
    0x41f000f4,
    0x0000b06c,
    0x000080f4,
    0x0000b06e,
    0x01f04004,
    0x0000b0ac,
    0x00000004,
    0xffffb0ae,
}};
