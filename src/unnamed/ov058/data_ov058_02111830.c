#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov058_02111830_Layout;
typedef char data_ov058_02111830_size[sizeof(data_ov058_02111830_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov058_02111834_Layout;
typedef char data_ov058_02111834_size[sizeof(data_ov058_02111834_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[13];
} data_ov058_02111844_Layout;
typedef char data_ov058_02111844_size[sizeof(data_ov058_02111844_Layout) == 0x34 ? 1 : -1];

typedef struct {
    u32 w[18];
} data_ov058_02111878_Layout;
typedef char data_ov058_02111878_size[sizeof(data_ov058_02111878_Layout) == 0x48 ? 1 : -1];

typedef struct {
    u32 w[19];
} data_ov058_021118c0_Layout;
typedef char data_ov058_021118c0_size[sizeof(data_ov058_021118c0_Layout) == 0x4c ? 1 : -1];

extern data_ov058_02111830_Layout data_ov058_02111830;
extern data_ov058_02111834_Layout data_ov058_02111834;
extern data_ov058_02111844_Layout data_ov058_02111844;
extern data_ov058_02111878_Layout data_ov058_02111878;
extern data_ov058_021118c0_Layout data_ov058_021118c0;

data_ov058_02111830_Layout data_ov058_02111830 = {{
    0x0357ff02,
}};

data_ov058_02111834_Layout data_ov058_02111834 = {{
    0xf97b0320,
    0x60000000,
    0x00040000,
    0x04e20f19,
}};

data_ov058_02111844_Layout data_ov058_02111844 = {{
    0x0000ee82,
    0x00004000,
    0x0c120004,
    0xf47a047e,
    0x6e380000,
    0xff020000,
    0x03200d48,
    0x0000f1f0,
    0x00008000,
    0x0578ff02,
    0xf76802ee,
    0x60000000,
    0x00000000,
}};

data_ov058_02111878_Layout data_ov058_02111878 = {{
    0x02bc1372,
    0x0000f002,
    0x002ca000,
    0x08fc2203,
    0xfa240302,
    0xa0000000,
    0x3200041d,
    0x02580275,
    0x0000fa5f,
    0x0f026000,
    0x09ee11ff,
    0xeea402bc,
    0x20000000,
    0x2202002b,
    0x02bc09ee,
    0x0000eea4,
    0x002b2000,
    0x00002202,
}};

data_ov058_021118c0_Layout data_ov058_021118c0 = {{
    0x03b10000, 0xf92a0258, 0x60000000, 0x07000000, 0x01ee0000, 0xf76d0352, 0x4e380000,
    0x05f80000, 0x09ee0000, 0xeea40352, 0x20000000, 0x05f90000, 0x13720000, 0xf0020352,
    0xa0000000, 0x05fa0000, 0x0d5c0000, 0xef660226, 0xe0000000,
}};
