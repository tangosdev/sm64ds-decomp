#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov039_02111400_Layout;
typedef char data_ov039_02111400_size[sizeof(data_ov039_02111400_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov039_02111404_Layout;
typedef char data_ov039_02111404_size[sizeof(data_ov039_02111404_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov039_02111408_Layout;
typedef char data_ov039_02111408_size[sizeof(data_ov039_02111408_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov039_0211140c_Layout;
typedef char data_ov039_0211140c_size[sizeof(data_ov039_0211140c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov039_02111414_Layout;
typedef char data_ov039_02111414_size[sizeof(data_ov039_02111414_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
} data_ov039_0211141c_Layout;
typedef char data_ov039_0211141c_size[sizeof(data_ov039_0211141c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
} data_ov039_02111428_Layout;
typedef char data_ov039_02111428_size[sizeof(data_ov039_02111428_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    u32 f00c;
    void *f010;
    u32 f014;
    u32 f018;
} data_ov039_02111438_Layout;
typedef char data_ov039_02111438_size[sizeof(data_ov039_02111438_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov039_02111454_Layout;
typedef char data_ov039_02111454_size[sizeof(data_ov039_02111454_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
    u32 f010;
    void *f014;
    u32 f018;
    void *f01c;
    u32 f020;
    void *f024;
} data_ov039_02111470_Layout;
typedef char data_ov039_02111470_size[sizeof(data_ov039_02111470_Layout) == 0x28 ? 1 : -1];

typedef struct {
    u32 w[16];
} data_ov039_02111498_Layout;
typedef char data_ov039_02111498_size[sizeof(data_ov039_02111498_Layout) == 0x40 ? 1 : -1];

extern data_ov039_02111400_Layout data_ov039_02111400;
extern data_ov039_02111404_Layout data_ov039_02111404;
extern data_ov039_02111408_Layout data_ov039_02111408;
extern data_ov039_0211140c_Layout data_ov039_0211140c;
extern data_ov039_02111414_Layout data_ov039_02111414;
extern data_ov039_0211141c_Layout data_ov039_0211141c;
extern data_ov039_02111428_Layout data_ov039_02111428;
extern data_ov039_02111438_Layout data_ov039_02111438;
extern data_ov039_02111454_Layout data_ov039_02111454;
extern data_ov039_02111470_Layout data_ov039_02111470;
extern data_ov039_02111498_Layout data_ov039_02111498;
extern u8 data_ov039_02111518[];
extern u8 data_ov039_02111658[];
extern u8 data_ov039_021114d8[];

data_ov039_02111408_Layout data_ov039_02111408 = {{
    0x00000014,
}};

data_ov039_02111404_Layout data_ov039_02111404 = {{
    0x00000ed8,
}};

data_ov039_02111400_Layout data_ov039_02111400 = {{
    0x0000074c,
}};

data_ov039_02111414_Layout data_ov039_02111414 = {
    0x00000002,
    &data_ov039_02111428,
};

data_ov039_0211140c_Layout data_ov039_0211140c = {
    0x00000005,
    &data_ov039_02111470,
};

data_ov039_0211141c_Layout data_ov039_0211141c = {
    &data_ov039_02111414,
    0x00000000,
    0x1b580000,
};

data_ov039_02111428_Layout data_ov039_02111428 = {
    0x00001400,
    &data_ov039_02111518[0],
    0x00003805,
    &data_ov039_02111658[0],
};

data_ov039_02111454_Layout data_ov039_02111454 = {{
    0xec78ff02,
    0x00000bb8,
    0x40000000,
    0xff070000,
    0xff380000,
    0x00000000,
    0x00000000,
}};

data_ov039_02111438_Layout data_ov039_02111438 = {
    &data_ov039_021114d8[0], &data_ov039_0211140c, 0x074b074d, 0x074f074e,
    &data_ov039_0211141c,    0x03e80001,           0x0000002f,
};

data_ov039_02111470_Layout data_ov039_02111470 = {
    0x00000204, &data_ov039_02111454, 0x0000010b, &data_ov039_02111400,
    0x00000401, &data_ov039_02111498, 0x0000010c, &data_ov039_02111404,
    0x0000010e, &data_ov039_02111408,
};

data_ov039_02111498_Layout data_ov039_02111498 = {{
    0xf0600000,
    0x00000b32,
    0x40000000,
    0x01000000,
    0x02260000,
    0x00c8f830,
    0xc0000000,
    0x00780000,
    0x02260000,
    0xff38f830,
    0xc0000000,
    0x00780000,
    0x02260000,
    0xfda8f830,
    0xc0000000,
    0x00780000,
}};
