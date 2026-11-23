#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov052_02111928_Layout;
typedef char data_ov052_02111928_size[sizeof(data_ov052_02111928_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov052_0211192c_Layout;
typedef char data_ov052_0211192c_size[sizeof(data_ov052_0211192c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov052_02111944_Layout;
typedef char data_ov052_02111944_size[sizeof(data_ov052_02111944_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[13];
} data_ov052_02111960_Layout;
typedef char data_ov052_02111960_size[sizeof(data_ov052_02111960_Layout) == 0x34 ? 1 : -1];

typedef struct {
    u32 w[46];
} data_ov052_02111994_Layout;
typedef char data_ov052_02111994_size[sizeof(data_ov052_02111994_Layout) == 0xb8 ? 1 : -1];

extern const data_ov052_02111928_Layout data_ov052_02111928;
extern const data_ov052_0211192c_Layout data_ov052_0211192c;
extern const data_ov052_02111944_Layout data_ov052_02111944;
extern const data_ov052_02111960_Layout data_ov052_02111960;
extern const data_ov052_02111994_Layout data_ov052_02111994;

const data_ov052_02111928_Layout data_ov052_02111928 = {{
    0x00000361,
}};

const data_ov052_0211192c_Layout data_ov052_0211192c = {{
    0x00000377,
    0x0000038e,
    0x000003a5,
    0x000003bc,
    0x000003d2,
    0x000003e9,
}};

const data_ov052_02111944_Layout data_ov052_02111944 = {{
    0x00000400,
    0x00000417,
    0x0000042e,
    0x00000444,
    0x0000045b,
    0x00000472,
    0x00000489,
}};

const data_ov052_02111960_Layout data_ov052_02111960 = {{
    0x0000049f,
    0x000004b6,
    0x000004cd,
    0x000004e4,
    0x000004fa,
    0x00000511,
    0x00000528,
    0x0000053f,
    0x00000555,
    0x0000056c,
    0x00000583,
    0x0000059a,
    0x000005b0,
}};

const data_ov052_02111994_Layout data_ov052_02111994 = {{
    0x000005c7, 0x000005de, 0x000005f5, 0x0000060b, 0x00000622, 0x00000639, 0x00000650, 0x00000666,
    0x0000067d, 0x00000694, 0x000006ab, 0x000006c1, 0x000006d8, 0x000006ef, 0x00000706, 0x0000071c,
    0x00000733, 0x0000074a, 0x00000761, 0x00000777, 0x0000078e, 0x000007a5, 0x000007bc, 0x000007d2,
    0x000007e9, 0x00000800, 0x00000817, 0x0000082e, 0x00000844, 0x0000085b, 0x00000872, 0x00000889,
    0x0000089f, 0x000008b6, 0x000008cd, 0x000008e4, 0x000008fa, 0x00000911, 0x00000928, 0x0000093f,
    0x00000955, 0x0000096c, 0x00000983, 0x0000099a, 0x000009b0, 0x000009c7,
}};
