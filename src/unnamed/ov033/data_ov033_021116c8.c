#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov033_021116c8_Layout;
typedef char data_ov033_021116c8_size[sizeof(data_ov033_021116c8_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov033_021116cc_Layout;
typedef char data_ov033_021116cc_size[sizeof(data_ov033_021116cc_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov033_021116d0_Layout;
typedef char data_ov033_021116d0_size[sizeof(data_ov033_021116d0_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov033_021116d4_Layout;
typedef char data_ov033_021116d4_size[sizeof(data_ov033_021116d4_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov033_021116d8_Layout;
typedef char data_ov033_021116d8_size[sizeof(data_ov033_021116d8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[5];
} data_ov033_021116e0_Layout;
typedef char data_ov033_021116e0_size[sizeof(data_ov033_021116e0_Layout) == 0x14 ? 1 : -1];

extern const data_ov033_021116c8_Layout data_ov033_021116c8;
extern const data_ov033_021116cc_Layout data_ov033_021116cc;
extern const data_ov033_021116d0_Layout data_ov033_021116d0;
extern const data_ov033_021116d4_Layout data_ov033_021116d4;
extern const data_ov033_021116d8_Layout data_ov033_021116d8;
extern const data_ov033_021116e0_Layout data_ov033_021116e0;
extern u8 data_ov033_02111b14[];

const data_ov033_021116c8_Layout data_ov033_021116c8 = {{
    0x00000000,
}};

const data_ov033_021116d4_Layout data_ov033_021116d4 = {{
    0x00001000,
}};

const data_ov033_021116d0_Layout data_ov033_021116d0 = {{
    0x00001000,
}};

const data_ov033_021116cc_Layout data_ov033_021116cc = {{
    0x00000000,
}};

const data_ov033_021116d8_Layout data_ov033_021116d8 = {
    0x0000ffff,
    &data_ov033_02111b14[0],
};

const data_ov033_021116e0_Layout data_ov033_021116e0 = {{
    0x00000001,
    0x00000001,
    0x00000001,
    0x0000005b,
    0x0000005b,
}};
