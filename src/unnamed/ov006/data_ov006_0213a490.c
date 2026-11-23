#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov006_0213a490_Layout;
typedef char data_ov006_0213a490_size[sizeof(data_ov006_0213a490_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0213a498_Layout;
typedef char data_ov006_0213a498_size[sizeof(data_ov006_0213a498_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0213a4a0_Layout;
typedef char data_ov006_0213a4a0_size[sizeof(data_ov006_0213a4a0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0213a4a8_Layout;
typedef char data_ov006_0213a4a8_size[sizeof(data_ov006_0213a4a8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
} sAmidaPieceSprites_Layout;
typedef char sAmidaPieceSprites_size[sizeof(sAmidaPieceSprites_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
} sAmidaLaneResultSprites_Layout;
typedef char sAmidaLaneResultSprites_size[sizeof(sAmidaLaneResultSprites_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_0213a4d0_Layout;
typedef char data_ov006_0213a4d0_size[sizeof(data_ov006_0213a4d0_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_0213a4e8_Layout;
typedef char data_ov006_0213a4e8_size[sizeof(data_ov006_0213a4e8_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_0213a500_Layout;
typedef char data_ov006_0213a500_size[sizeof(data_ov006_0213a500_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov006_0213a520_Layout;
typedef char data_ov006_0213a520_size[sizeof(data_ov006_0213a520_Layout) == 0x30 ? 1 : -1];

extern data_ov006_0213a490_Layout data_ov006_0213a490;
extern data_ov006_0213a498_Layout data_ov006_0213a498;
extern data_ov006_0213a4a0_Layout data_ov006_0213a4a0;
extern data_ov006_0213a4a8_Layout data_ov006_0213a4a8;
extern sAmidaPieceSprites_Layout sAmidaPieceSprites;
extern sAmidaLaneResultSprites_Layout sAmidaLaneResultSprites;
extern data_ov006_0213a4d0_Layout data_ov006_0213a4d0;
extern data_ov006_0213a4e8_Layout data_ov006_0213a4e8;
extern data_ov006_0213a500_Layout data_ov006_0213a500;
extern data_ov006_0213a520_Layout data_ov006_0213a520;

#pragma define_section sName ".data.gap38"                                                         \
                             ".data.gap38"                                                         \
                             ".data.gap38" far_data
#pragma section sName begin

data_ov006_0213a4a0_Layout data_ov006_0213a4a0 = {{
    0x01f800f8,
    0xffff003f,
}};

data_ov006_0213a4a8_Layout data_ov006_0213a4a8 = {{
    0x01f800f8,
    0xffff001f,
}};

data_ov006_0213a498_Layout data_ov006_0213a498 = {{
    0x01f800f8,
    0xffff005f,
}};

data_ov006_0213a490_Layout data_ov006_0213a490 = {{
    0x01f800f8,
    0xffff007f,
}};

sAmidaPieceSprites_Layout sAmidaPieceSprites = {
    &data_ov006_0213a4a8,
    &data_ov006_0213a4a0,
    &data_ov006_0213a498,
    &data_ov006_0213a490,
};

sAmidaLaneResultSprites_Layout sAmidaLaneResultSprites = {
    &data_ov006_0213a500,
    &data_ov006_0213a4e8,
    &data_ov006_0213a520,
    &data_ov006_0213a4d0,
};

data_ov006_0213a4e8_Layout data_ov006_0213a4e8 = {{
    0x81f0000e,
    0x000020c6,
    0x01f44028,
    0x000040dc,
    0x00040028,
    0xffff40df,
}};

data_ov006_0213a4d0_Layout data_ov006_0213a4d0 = {{
    0x81e8000e,
    0x000040d2,
    0x8008800e,
    0x000040d6,
    0x41f04028,
    0xffff40dc,
}};

data_ov006_0213a500_Layout data_ov006_0213a500 = {{
    0x81e8000e,
    0x000010c0,
    0x8008800e,
    0x000010c4,
    0x01f44028,
    0x000040dc,
    0x00040028,
    0xffff40df,
}};

data_ov006_0213a520_Layout data_ov006_0213a520 = {{
    0xc1e04006,
    0x000030aa,
    0x41e04026,
    0x0000312a,
    0x40004026,
    0x0000312e,
    0x01e84028,
    0x000040dc,
    0x00084028,
    0x000040de,
    0x01f84028,
    0xffff40dd,
}};
