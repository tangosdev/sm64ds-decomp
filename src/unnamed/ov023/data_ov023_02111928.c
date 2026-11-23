#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov023_02111928_Layout;
typedef char data_ov023_02111928_size[sizeof(data_ov023_02111928_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov023_0211192c_Layout;
typedef char data_ov023_0211192c_size[sizeof(data_ov023_0211192c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov023_02111944_Layout;
typedef char data_ov023_02111944_size[sizeof(data_ov023_02111944_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[13];
} data_ov023_02111960_Layout;
typedef char data_ov023_02111960_size[sizeof(data_ov023_02111960_Layout) == 0x34 ? 1 : -1];

typedef struct {
    u32 w[46];
} data_ov023_02111994_Layout;
typedef char data_ov023_02111994_size[sizeof(data_ov023_02111994_Layout) == 0xb8 ? 1 : -1];

extern const data_ov023_02111928_Layout data_ov023_02111928;
extern const data_ov023_0211192c_Layout data_ov023_0211192c;
extern const data_ov023_02111944_Layout data_ov023_02111944;
extern const data_ov023_02111960_Layout data_ov023_02111960;
extern const data_ov023_02111994_Layout data_ov023_02111994;

const data_ov023_02111928_Layout data_ov023_02111928 = {{
    0xfffff7a3,
}};

const data_ov023_0211192c_Layout data_ov023_0211192c = {{
    0xfffff78f,
    0xfffff77a,
    0xfffff766,
    0xfffff751,
    0xfffff73c,
    0xfffff728,
}};

const data_ov023_02111944_Layout data_ov023_02111944 = {{
    0xfffff713,
    0xfffff6ff,
    0xfffff6ea,
    0xfffff6d6,
    0xfffff6c1,
    0xfffff6ac,
    0xfffff698,
}};

const data_ov023_02111960_Layout data_ov023_02111960 = {{
    0xfffff683,
    0xfffff66f,
    0xfffff65a,
    0xfffff645,
    0xfffff631,
    0xfffff61c,
    0xfffff608,
    0xfffff5f3,
    0xfffff5df,
    0xfffff5ca,
    0xfffff5b5,
    0xfffff5a1,
    0xfffff58c,
}};

const data_ov023_02111994_Layout data_ov023_02111994 = {{
    0xfffff578, 0xfffff563, 0xfffff54e, 0xfffff53a, 0xfffff525, 0xfffff511, 0xfffff4fc, 0xfffff4e8,
    0xfffff4d3, 0xfffff4be, 0xfffff4aa, 0xfffff495, 0xfffff481, 0xfffff46c, 0xfffff457, 0xfffff443,
    0xfffff42e, 0xfffff41a, 0xfffff405, 0xfffff3f1, 0xfffff3dc, 0xfffff3c7, 0xfffff3b3, 0xfffff39e,
    0xfffff38a, 0xfffff375, 0xfffff360, 0xfffff34c, 0xfffff337, 0xfffff323, 0xfffff30e, 0xfffff2fa,
    0xfffff2e5, 0xfffff2d0, 0xfffff2bc, 0xfffff2a7, 0xfffff293, 0xfffff27e, 0xfffff269, 0xfffff255,
    0xfffff240, 0xfffff22c, 0xfffff217, 0xfffff203, 0xfffff1ee, 0xfffff1d9,
}};
