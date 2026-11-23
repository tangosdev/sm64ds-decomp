#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov039_0211153c_Layout;
typedef char data_ov039_0211153c_size[sizeof(data_ov039_0211153c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[5];
} data_ov039_02111548_Layout;
typedef char data_ov039_02111548_size[sizeof(data_ov039_02111548_Layout) == 0x14 ? 1 : -1];

extern data_ov039_0211153c_Layout data_ov039_0211153c;
extern data_ov039_02111548_Layout data_ov039_02111548;

data_ov039_0211153c_Layout data_ov039_0211153c = {{
    0x1770f8c6,
    0x00000000,
    0xffff0000,
}};

data_ov039_02111548_Layout data_ov039_02111548 = {{
    0x0bb80106,
    0x157cfcae,
    0x00000000,
    0xffff0000,
    0x03e80106,
}};
