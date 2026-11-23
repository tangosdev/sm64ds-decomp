#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov058_02111928_Layout;
typedef char data_ov058_02111928_size[sizeof(data_ov058_02111928_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov058_0211192c_Layout;
typedef char data_ov058_0211192c_size[sizeof(data_ov058_0211192c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov058_02111944_Layout;
typedef char data_ov058_02111944_size[sizeof(data_ov058_02111944_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[13];
} data_ov058_02111960_Layout;
typedef char data_ov058_02111960_size[sizeof(data_ov058_02111960_Layout) == 0x34 ? 1 : -1];

extern data_ov058_02111928_Layout data_ov058_02111928;
extern data_ov058_0211192c_Layout data_ov058_0211192c;
extern data_ov058_02111944_Layout data_ov058_02111944;
extern data_ov058_02111960_Layout data_ov058_02111960;

data_ov058_02111928_Layout data_ov058_02111928 = {{
    0x00000fc0,
}};

data_ov058_0211192c_Layout data_ov058_0211192c = {{
    0x000000ff,
    0x00030fc4,
    0x000000ff,
    0x00003044,
    0x000000ff,
    0x04000fc0,
}};

data_ov058_02111944_Layout data_ov058_02111944 = {{
    0x000000ff,
    0x00033045,
    0x000000ff,
    0x00033042,
    0x000000ff,
    0x00030042,
    0x000000ff,
}};

data_ov058_02111960_Layout data_ov058_02111960 = {{
    0x00030045,
    0x000000ff,
    0x00030040,
    0x000000ff,
    0x00000fc4,
    0x000000ff,
    0x00003084,
    0x000000ff,
    0x00030085,
    0x000000ff,
    0x00030084,
    0x000000ff,
    0x00030fc1,
}};
