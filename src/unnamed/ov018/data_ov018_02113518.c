#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[7];
} data_ov018_02113518_Layout;
typedef char data_ov018_02113518_size[sizeof(data_ov018_02113518_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov018_02113534_Layout;
typedef char data_ov018_02113534_size[sizeof(data_ov018_02113534_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[13];
} data_ov018_02113554_Layout;
typedef char data_ov018_02113554_size[sizeof(data_ov018_02113554_Layout) == 0x34 ? 1 : -1];

typedef struct {
    u32 w[19];
} data_ov018_02113588_Layout;
typedef char data_ov018_02113588_size[sizeof(data_ov018_02113588_Layout) == 0x4c ? 1 : -1];

extern data_ov018_02113518_Layout data_ov018_02113518;
extern data_ov018_02113534_Layout data_ov018_02113534;
extern data_ov018_02113554_Layout data_ov018_02113554;
extern data_ov018_02113588_Layout data_ov018_02113588;

data_ov018_02113518_Layout data_ov018_02113518 = {{
    0x0c1c443f,
    0x141eeed0,
    0xf3e48226,
    0xf7ccfb4e,
    0xf18c8226,
    0x189cf7cc,
    0x0ac28226,
}};

data_ov018_02113534_Layout data_ov018_02113534 = {{
    0x09f8fa7d,
    0x0a8c8226,
    0x04b0ee18,
    0x10cc8226,
    0xfa24ee6d,
    0xfc6d8e26,
    0x0862fe70,
    0x1004083f,
}};

data_ov018_02113554_Layout data_ov018_02113554 = {{
    0x0190fb82,
    0xfbb4e03d,
    0xf73609c4,
    0xfcc48e26,
    0xfc6c0da2,
    0x0d67ce3f,
    0x1193ed40,
    0xf2e1ff0f,
    0x04fffa83,
    0xff4ce63d,
    0xfa5e0ed2,
    0x04c48226,
    0x14e8f128,
}};

data_ov018_02113588_Layout data_ov018_02113588 = {{
    0x022500e5, 0x1880ef02, 0x00000000, 0x00010000, 0xf6a0004c, 0xfc7c0ab2, 0x8000ffff,
    0x047e0000, 0xfc18004c, 0xf8f809c4, 0xc000ffff, 0x04800000, 0xff64009d, 0xfc7c0d3e,
    0x80000000, 0x00000000, 0x09c40103, 0xf9c009c3, 0x00000000,
}};
