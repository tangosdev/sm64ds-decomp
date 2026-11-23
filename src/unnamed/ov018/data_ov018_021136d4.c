#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[8];
} data_ov018_021136d4_Layout;
typedef char data_ov018_021136d4_size[sizeof(data_ov018_021136d4_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[18];
} data_ov018_021136f4_Layout;
typedef char data_ov018_021136f4_size[sizeof(data_ov018_021136f4_Layout) == 0x48 ? 1 : -1];

typedef struct {
    u32 w[42];
} data_ov018_0211373c_Layout;
typedef char data_ov018_0211373c_size[sizeof(data_ov018_0211373c_Layout) == 0xa8 ? 1 : -1];

extern data_ov018_021136d4_Layout data_ov018_021136d4;
extern data_ov018_021136f4_Layout data_ov018_021136f4;
extern data_ov018_0211373c_Layout data_ov018_0211373c;

data_ov018_021136d4_Layout data_ov018_021136d4 = {{
    0xffff0000,
    0xef34004c,
    0xeda4ed6f,
    0xc000ffff,
    0x07430000,
    0xecdc0103,
    0xf380edc6,
    0x00000000,
}};

data_ov018_021136f4_Layout data_ov018_021136f4 = {{
    0xffff0000,
    0xecdc0103,
    0xf8f8ee12,
    0x00000000,
    0xffff0000,
    0xf2360032,
    0x1252ee6c,
    0x00000000,
    0x00024000,
    0xf9c0001f,
    0x06a4ff06,
    0x00000000,
    0xff020000,
    0x0d69004d,
    0x1194ee49,
    0x00000000,
    0xffff0000,
    0xec290031,
}};

data_ov018_0211373c_Layout data_ov018_0211373c = {{
    0x0b6cfbb4, 0x00000000, 0x00170000, 0xec290031, 0x0b6cfbb4, 0x00000000, 0x00f80000,
    0x042e001d, 0x15e0ee6c, 0x00000000, 0xff010000, 0xf236001d, 0x1252ee6c, 0x00000000,
    0xffff0000, 0xecbe001d, 0x1194f95c, 0x00000000, 0xffff0000, 0xef6200fe, 0xfdaafae4,
    0x80000000, 0x01040000, 0xea5c00fe, 0x0d13f95c, 0x80000000, 0x02040000, 0xfc69011d,
    0x0863fe0c, 0xf3340000, 0xffff0000, 0xfc69011d, 0x0863ff38, 0xf3340000, 0xffff0000,
    0xfc69011d, 0x08630064, 0xf3340000, 0xffff0000, 0xff1a0108, 0x0924fe70, 0x00000000,
}};
