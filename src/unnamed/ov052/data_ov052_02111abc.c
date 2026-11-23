#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov052_02111abc_Layout;
typedef char data_ov052_02111abc_size[sizeof(data_ov052_02111abc_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov052_02111ad4_Layout;
typedef char data_ov052_02111ad4_size[sizeof(data_ov052_02111ad4_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov052_02111af4_Layout;
typedef char data_ov052_02111af4_size[sizeof(data_ov052_02111af4_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov052_02111b14_Layout;
typedef char data_ov052_02111b14_size[sizeof(data_ov052_02111b14_Layout) == 0x20 ? 1 : -1];

extern const data_ov052_02111abc_Layout data_ov052_02111abc;
extern const data_ov052_02111ad4_Layout data_ov052_02111ad4;
extern const data_ov052_02111af4_Layout data_ov052_02111af4;
extern const data_ov052_02111b14_Layout data_ov052_02111b14;

const data_ov052_02111abc_Layout data_ov052_02111abc = {{
    0x00000c5b,
    0x00000c72,
    0x00000c89,
    0x00000c9f,
    0x00000cb6,
    0x00000ccd,
}};

const data_ov052_02111ad4_Layout data_ov052_02111ad4 = {{
    0x00000ce4,
    0x00000cfa,
    0x00000d11,
    0x00000d28,
    0x00000d3f,
    0x00000d55,
    0x00000d6c,
    0x00000d83,
}};

const data_ov052_02111af4_Layout data_ov052_02111af4 = {{
    0x00000d9a,
    0x00000db0,
    0x00000dc7,
    0x00000dde,
    0x00000df5,
    0x00000e0b,
    0x00000e22,
    0x00000e39,
}};

const data_ov052_02111b14_Layout data_ov052_02111b14 = {{
    0x00000e50,
    0x00000e66,
    0x00000e7d,
    0x00000e94,
    0x00000eab,
    0x00000ec1,
    0x00000ed8,
    0x00000eef,
}};
